<?php

/**
 * @file
 * This file describes the MM plugin as implemented by eDoc.
 */

/**
 * Plugin definition with callbacks.
 */
$plugin = array(
  'description' => t('eDoc ESDH MM Backend'),
  'version' => 1,
  'test' => 'os2web_edoc_esdh_mm_test',
  'get_import_list' => 'os2web_edoc_esdh_mm_get_import_list',
  'import_meeting' => 'os2web_edoc_esdh_mm_import_meeting',
  'post_import_process' => '_os2web_edoc_esdh_mm_order_addenums',
);

/**
 * For plugin testing purposes.
 */
function os2web_edoc_esdh_mm_test($stuff) {
  return 'Test successful. Recieved: ' . $stuff;
}

/**
 * Gets the Plugin ID.
 *
 * Plugin ID must be same as the filename with no exception.
 */
function os2web_edoc_esdh_mm_get_plugin_id() {
  return 'os2web_edoc_esdh_mm';
}

/**
 * Returns a list of meeting_ids for import.
 *
 * In our case a list of uri's of xml files.
 *
 * @return array
 *   List of URIs
 */
function os2web_edoc_esdh_mm_get_import_list() {
  /*
   * For the eDoc service, we need to import meetings from xml files.
   * We will enumerate the list with the publication-id
   * as parsed from the dir name of the manifest.
   */
  $uris = array();
  foreach (file_scan_directory(variable_get('os2web_edoc_esdh_mm_path', OS2WEB_EDOC_ESDH_MM_IMPORT_DIR), '/\.xml$/i', array('recurse' => TRUE)) as $xml_file) {
    $committee = NULL;
    $meeting_date = NULL;

    $real_path = drupal_realpath($xml_file->uri);
    if ($real_path !== FALSE && is_file($real_path)) {
      libxml_use_internal_errors(TRUE);
      $manifest = simplexml_load_string(utf8_for_xml(file_get_contents($real_path)), 'SimpleXMLElement', LIBXML_NOWARNING);
      libxml_clear_errors();
      libxml_use_internal_errors(FALSE);

      if (is_object($manifest)) {
        $committee_xpath = $manifest->xpath("/Meeting/Committee");
        $committee = (string) array_shift($committee_xpath);
        $meeting_date_xpath = $manifest->xpath("/Meeting/MeetingDate");
        $meeting_date = (string) array_shift($meeting_date_xpath);
      }
    }

    $uris[] = array(
      'id' => _os2web_edoc_esdh_generate_meeting_id($meeting_date, $committee),
      'uri' => $xml_file->uri,
      'plugin_id' => os2web_edoc_esdh_mm_get_plugin_id(),
    );
  }
  return $uris;
}

/**
 * Strip illegal characters from the XML.
 *
 * Ex &#xC = ASCII new line feed.
 *
 * @param string $string
 *   String to sanitise.
 *
 * @return string
 *   The string sanitised.
 */
function utf8_for_xml($string) {
  // Using regex.
  if (variable_get('os2web_edoc_esdh_banned_activate_regex', FALSE)) {
    $replace_me = variable_get('os2web_edoc_esdh_banned_special_regex', '');
    $str_new = preg_replace($replace_me, "", $string);
    // Using CSV list.
  }
  else {
    $replace_me = variable_get('os2web_edoc_esdh_banned_special_char', '');
    $str_new = str_replace(explode(',', $replace_me), "", $string);
  }
  return $str_new;
}

/**
 * Imports a meeting, and return the import as a structured array.
 *
 * @param object $meeting
 *   Meeting XML object.
 *
 * @return array
 *   List of meeting_structures to save
 */
function os2web_edoc_esdh_mm_import_meeting($meeting) {
  $file = $meeting['uri'];
  if ($file === FALSE) {
    _os2web_edoc_esdh_mm_order_addenums();
  }
  $meetings = array();
  $real_path = drupal_realpath($file);
  if ($real_path !== FALSE && is_file($real_path)) {
    libxml_use_internal_errors(TRUE);
    $manifest = simplexml_load_string(utf8_for_xml(file_get_contents($real_path)), 'SimpleXMLElement', LIBXML_NOWARNING);

    libxml_clear_errors();
    libxml_use_internal_errors(FALSE);

    if ($manifest === FALSE) {
      error_log("Failed loading XML");
      foreach (libxml_get_errors() as $error) {
        error_log($error->message);
      }
    }
    if (is_object($manifest)) {
      // Collecting agendas.
      $meeting_agendas = $manifest->xpath("/Meeting/MeetingAgendaTypes/MeetingAgendaType");
      foreach ($meeting_agendas as $meeting_agenda) {
        // Collecting agenda data.
        $committee_xpath = $manifest->xpath("/Meeting/Committee");
        $committee = (string) array_shift($committee_xpath);
        $meeting_date_xpath = $manifest->xpath("/Meeting/MeetingDate");
        $meeting_date = (string) array_shift($meeting_date_xpath);
        // Generated meeting_id.
        $meeting_id = _os2web_edoc_esdh_generate_meeting_id($meeting_date, $committee);
        $meeting_pdf = $meeting_agenda->xpath('PDFDocument');
        $meeting_pdf = (string) $meeting_pdf[0];
        $meeting_state = $meeting_agenda->xpath('AgendaType');
        $meeting_state = (string) $meeting_state[0];
        $meeting_type = $manifest->xpath('/Meeting/MeetingState');
        $meeting_type = (string) $meeting_type[0];
        $acadre_version_xpath = explode('.', $meeting_pdf);
        $acadre_version = (string) array_shift($acadre_version_xpath);

        if (stripos($meeting_state, 'lukket') !== FALSE) {
          $agenda_access = 'Lukket';
        }
        else {
          $agenda_access = 'Åben';
        }

        // Collecting agenda items.
        $meeting_agendas_items = $meeting_agenda->xpath('MeetingAgendaItems');

        // Fix dates.
        $date_obj = date_create($meeting_date, new DateTimeZone(date_default_timezone()));
        date_timezone_set($date_obj, timezone_open('UTC'));
        $meeting_date = date_format($date_obj, 'Y-m-d H:i');

        // Changing the meeting state to match ESDH Provider module.
        if ($meeting_type === "Referat endeligt godkendt") {
          $meeting_type = OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_REFERAT;
        }
        else {
          $meeting_type = OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_DAGSORDEN;
        }

        // Saving the meeting data in ESDH Provider format.
        $meeting = array(
          'system_id' => $meeting_id,
          'meeting_id' => $meeting_id,
          'meeting_sub_id' => $meeting_id,
          'committee' => $committee,
          'committee_name' => $committee,
          'committee_id' => _os2web_edoc_esdh_generate_committee_id($committee),
          'title' => $committee,
          'uri' => strstr($file, 'eDocAgendaPublishing.xml', TRUE),
          'path' => drupal_realpath($file),
          'meeting_date' => $meeting_date,
          'meeting_date_start' => $meeting_date,
          'meeting_date_finish' => $meeting_date,
          'meeting_type' => $meeting_type,
          'type' => $meeting_type,
          'meeting_state' => $meeting_state,
          'meeting_pdf' => $meeting_pdf,
          'acadre_version' => $acadre_version,
          'agenda_access' => $agenda_access,
        );

        // Error logging to watchdog if the agenda item list is empty.
        if (empty($meeting_agendas_items)) {
          watchdog('eDoc MM', 'Empty list of import items in %file.', array('%file' => $file), WATCHDOG_WARNING);
        }

        foreach ($meeting_agendas_items as $meeting_agenda_item) {
          // Importing each agenda item through the ESDH provider module.
          $imported = _os2web_edoc_esdh_mm_import_meeting_agenda($meeting, $meeting_agenda_item);
          if (is_array($imported)) {
            $imported['publish'] = TRUE;
            $meetings[] = $imported;
          }
        }
      }
    }
    else {
      // Error logging to watchdog if the XML failde to parse. Check if all
      // illegal characters are in the remove list.
      watchdog('eDoc MM', 'Failed to parse XML in %file during import.', array('%file' => $file), WATCHDOG_WARNING);
    }
  }
  return $meetings;
}

/**
 * Imports a meeting from filesystem.
 *
 * @param mixed $meeting
 *   Meeting array.
 * @param SimpleXMLElement $agenda_xml
 *   Meeting agenda XML object.
 *
 * @return mixed
 *   Meeting_struct array for an accepted import, else FALSE
 */
function _os2web_edoc_esdh_mm_import_meeting_agenda(&$meeting, SimpleXMLElement $agenda_xml) {
  // Check if the committee is allowed to publish.
  $publish_committee = array_map('trim', explode(',', variable_get('os2web_edoc_esdh_approved_committees', OS2WEB_EDOC_ESDH_MM_DEFAULT_APPROVED)));
  if ($meeting['committee'] === '' || !in_array($meeting['committee'], $publish_committee)) {
    watchdog('eDoc MM', 'Ignored agenda from "%committee" .', array('%committee' => $meeting['committee']));
    return FALSE;
  }

  $meeting['items'] = array();
  $agenda = $meeting;
  // Only add the agenda if its PDF exists.
  $meeting_full_pdf_path = $meeting['uri'] . $meeting['pdf'];
  $real_path = drupal_realpath($meeting_full_pdf_path);
  if ($real_path !== FALSE && file_exists($real_path)) {
    $agenda['full_doc'] = array(
      'uri' => $meeting_full_pdf_path,
      'title' => $meeting['committee'],
    );
    // Import all agenda items.
    $agenda_bullet = $agenda_xml;
    foreach ($agenda_bullet->MeetingAgendaItem as $agenda_item) {
      if (($item = _os2web_edoc_esdh_mm_import_meeting_agenda_item($meeting, $agenda_item)) !== FALSE) {
        $agenda['items'][] = $item;
      }
    }
    // Apply defaults, just in case we missed something.
    $agenda += os2web_esdh_provider_default_meeting();
    return $agenda;
  }
  watchdog('eDoc MM', 'File do not exist in folder. %file', array('%file' => $meeting_full_pdf_path), WATCHDOG_WARNING);
  return FALSE;
}

/**
 * Imports a single item for an agenda.
 *
 * @param mixed $meeting
 *   Meeting array.
 * @param SimpleXMLElement $xml
 *   Xml segment containing the item.
 *
 * @return array
 *   The imported item.
 */
function _os2web_edoc_esdh_mm_import_meeting_agenda_item($meeting, SimpleXMLElement $xml) {
  $xml_item = array();
  $xml_item['number'] = (int) $xml->AgendaItemNumber;
  $xml_item['pdf'] = (string) $xml->PDFDocument;
  $xml_item_document = $xml->Document;
  $xml_item_bullet = $xml_item_document->Attachments;
  $xml_item['name'] = (string) $xml_item_document->NeutralTitle;
  $xml_item['document_type'] = (string) $xml_item_document->PublishingType;
  $xml_item['agenda_type'] = (string) $meeting->AgendaType;
  $xml_item['documentid'] = (string) $xml_item_document['documentid'];
  if (empty($xml_item['documentid']) || $xml_item['documentid'] === '') {
    $pdf_name = explode('.', $xml_item['pdf']);
    $xml_item['documentid'] = $pdf_name[0];
  }

  $xml_item['full_text'] = trim((string) $xml->FullText);

  if (variable_get('os2web_edoc_esdh_nl2br', TRUE)) {
    $xml_item['full_text'] = nl2br($xml_item['full_text']);
  }

  $item = array(
    'id' => $xml_item['number'],
    'sysid' => _os2web_edoc_esdh_generate_item_id($meeting['committee'], $meeting['meeting_date'], $xml_item['documentid']),
    // Generated id - because there is not unique ID for the bullet point.
    'title' => 'Punkt ' . $xml_item['number'] . ' ' . $xml_item['name'],
    'access' => $xml_item['document_type'],
    'bullets' => array(),
    'enclosures' => array(),
    'doc' => array(
      'uri' => implode('/', array(
        trim($meeting['uri'], "/"),
        $xml_item['pdf'],
      )),
      'title' => $xml_item['name'],
    ),
  );

  if ($item['access'] !== 'SKAL PUBLICERES' || stripos($meeting['agenda_access'], 'lukket') !== FALSE) {
    $item['access'] = 'Lukket';
  }

  if ($item['access'] !== 'Lukket' || variable_get('os2web_edoc_esdh_import_closed_agenda', FALSE)) {
    if (variable_get('os2web_edoc_esdh_bullet_source', 'xml') === 'xml') {
      // Process item body.
      if (!empty($xml_item['full_text'])) {
        $item['bullets'][] = array(
          'title' => 'Beskrivelse',
          'body' => $xml_item['full_text'],
          'sysid' => $xml_item['documentid'] . $xml_item['number'],
        );
      }

      // Process bullets.
      foreach ($xml_item_bullet->Attachment as $bullet_xml) {
        $bullet_title = (string) $bullet_xml->NeutralTitle;
        $bullet_body = (string) trim($bullet_xml->FullText);
        $bullet_id = (string) $bullet_xml['attachmentid'];
        $bullet_access = (string) $bullet_xml->PublishingType;
        if (empty($bullet_body)) {
          $bullet_body = "Intet indhold";
        }
        else {
          if ($bullet_access !== 'SKAL PUBLICERES') {
            $bullet_body = "Dette er et lukket punkt. Du kan læse selve indstillingen" . $bullet_xml['access'];
          }
        }
        if (!empty($bullet_title)) {
          $item['bullets'][] = array(
            'title' => (string) $bullet_title,
            'body' => (string) $bullet_body,
            'sysid' => $bullet_id,
          );
        }

        // Process enclosure, if any.
        $enclosure_item = array();
        $enclosure_item['filename_in'] = (string) $bullet_xml->PDFDocument;
        $enclosure_item['access'] = (string) $bullet_xml->PublishingType;
        if (strlen($enclosure_item['filename_in']) > 0 && $enclosure_item['access'] === 'SKAL PUBLICERES') {
          $item['enclosures'][] = array(
            'uri' => implode('/', array(
              trim($meeting['uri'], "/"),
              $xml_item['pdf'],
            )),
            'title' => trim((string) $bullet_xml->Title),
            'sysid' => $bullet_id,
            'access' => TRUE,
            'filetype' => '.pdf',
            'type' => 'Bilag',
          );
        }
      }
    }
    else {
      // Skipping content and importing just one PDF with all attachment
      // gathered there.
      $item['enclosures'][] = array(
        'uri' => implode('/', array(
          trim($meeting['uri'], variable_get('os2web_edoc_esdh_mm_is_abs', FALSE) ? "" : "/"),
          $xml_item['pdf'],
        )),
        'title' => variable_get('os2web_edoc_esdh_empty_bullet_title', 'Indhold') . "_" . $xml_item['number'],
        'sysid' => $xml_item['documentid'],
        'access' => TRUE,
        'filetype' => '.pdf',
        'is_bp_summary' => TRUE,
      );
      foreach ($xml_item_bullet->Attachment as $bullet_xml) {
        $bullet_id = (string) $bullet_xml['attachmentid'];
        if (empty($bullet_id)) {
          $bullet_id = $xml_item['documentid'];
        }
        $enclosure_item = array();
        $enclosure_item['filename_in'] = (string) $bullet_xml->PDFDocument;
        $enclosure_item['access'] = (string) $bullet_xml->PublishingType;
        if (strlen($enclosure_item['filename_in']) > 0 && $enclosure_item['access'] === 'SKAL PUBLICERES') {
          $item['enclosures'][] = array(
            'uri' => implode('/', array(
              trim($meeting['uri'], variable_get('os2web_edoc_esdh_mm_is_abs', FALSE) ? "" : "/"),
              $enclosure_item['filename_in'],
            )),
            'title' => trim((string) $bullet_xml->Title),
            'sysid' => $bullet_id,
            'access' => TRUE,
            'filetype' => '.pdf',
            'type' => 'Bilag',
          );
        }
      }
    }
  }

  $item += os2web_esdh_provider_default_item();
  return $item;
}

/**
 * Case insensitive fileexists(). Code from comments on php.net.
 *
 * @param string $filename
 *   File name to check.
 *
 * @return mixed
 *   False if file not found, case corrected filename if found.
 */
function _os2web_edoc_esdh_similar_file_exists($filename) {
  if (file_exists($filename)) {
    return $filename;
  }
  $lcase_filename = strtolower($filename);
  foreach (glob(dirname($filename) . '/*') as $file) {
    if (strtolower($file) === $lcase_filename) {
      return $file;
    }
  }
  return FALSE;
}

/**
 * This function will set up the addenum connections for meetings.
 */
function _os2web_edoc_esdh_mm_order_addenums($drush = FALSE) {
  if (!lock_acquire(__FUNCTION__)) {
    return;
  }

  // Debug bit set in os2web_esdh_provider settings.
  $debug = variable_get('os2web_esdh_provider_debug_info');
  $debug && timer_start('_os2web_edoc_esdh_mm_order_addenums') && error_log('MM Import - Start of addendums - ' . timer_read('_os2web_edoc_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());

  $rev_meetings = array();

  // Find all meetings and gather meeting and agenda_ids.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node', '=')
    ->propertyCondition('type', 'os2web_meetings_meeting', '=');
  $nodes_result = $query->execute();

  if (!isset($nodes_result['node'])) {
    return;
  }
  $nodes = $nodes_result['node'];
  // Load necessary fields into the node object.
  $fields = field_info_instances('node', 'meeting');
  field_attach_load('node', $nodes, FIELD_LOAD_CURRENT, array('field_id' => $fields['field_os2web_meetings_addendum']['field_id']));
  field_attach_load('node', $nodes, FIELD_LOAD_CURRENT, array('field_id' => $fields['field_os2web_meetings_id']['field_id']));
  field_attach_load('node', $nodes, FIELD_LOAD_CURRENT, array('field_id' => $fields['field_os2web_meetings_sub_id']['field_id']));

  // Meetings as: $meetings[<meeting_id>][<system_id>][<node_id>] .
  $meetings = array();

  $nids_with_addendums = array();

  $debug && error_log('MM Import - Retrieve nodes with addendums from ' . count($nodes) . ' nodes - ' . timer_read('_os2web_edoc_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());
  // Loop through all meeting nodes, and generate an array with meeting grouped.
  foreach ($nodes as $node) {
    $addenum_to = field_get_items('node', $node, 'field_os2web_meetings_addendum');
    if (isset($addenum_to[0]['nid']) && is_numeric($addenum_to[0]['nid'])) {
      $nids_with_addendums[$node->nid] = $node->nid;
    }
    $meeting_id = field_get_items('node', $node, 'field_os2web_meetings_id');
    $meeting_sub_id = field_get_items('node', $node, 'field_os2web_meetings_sub_id');
    if (!isset($meeting_id[0]['value']) || !isset($meeting_sub_id[0]['value'])) {
      $debug && error_log('MM Import - Missing meeting_id or meeting_sub_id for nid=' . $node->nid);
      continue;
    }
    if (isset($meetings[$meeting_id[0]['value']][$meeting_sub_id[0]['value']])) {
      // Duplicate meeting detected.
      watchdog('eDoc MM', 'Duplicate import found for meeting %mid. Deleting nid=%nid.', array(
        '%mid' => $meeting_id['value'],
        '%nid' => $node->nid,
      ), WATCHDOG_WARNING);
      node_delete($node->nid);
    }
    else {
      $meetings[$meeting_id[0]['value']][$meeting_sub_id[0]['value']] = $node->nid;
    }
  }

  $debug && error_log('MM Import - Update addendums on ' . count($meetings) . ' nodes - ' . timer_read('_os2web_edoc_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());
  // Run through the meeting array, to determine the addendums.
  foreach ($meetings as $m_id => $meeting) {
    // Lowest system id first.
    ksort($meeting);
    if (in_array($m_id, $rev_meetings)) {
      $meeting = array_reverse($meeting);
    }
    // Lowest system id is primary.
    $primary = $nodes[array_shift($meeting)];
    $addenum_to = field_get_items('node', $primary, 'field_os2web_meetings_addendum');
    if (!empty($addenum_to[0]['nid'])) {
      $primary->field_os2web_meetings_addendum[LANGUAGE_NONE] = array();
      node_save($primary);
    }
    while ($next = array_shift($meeting)) {
      // Set up references for addenum agendas.
      $node = $nodes[$next];
      if ($primary) {
        $addenum_to = field_get_items('node', $node, 'field_os2web_meetings_addendum');
        if (is_array($addenum_to)) {
          $addenum_to = array_shift($addenum_to);
          $addenum_to = array_shift($addenum_to);
        }
        else {
          $addenum_to = FALSE;
        }

        // Save time by only updating what is needed.
        if ($addenum_to != $primary->nid) {
          $node->field_os2web_meetings_addendum[LANGUAGE_NONE][0]['nid'] = $primary->nid;
          node_save($node);
        }
      }
      else {
      }
      // Remove node from final nids_with_addendums.
      unset($nids_with_addendums[$node->nid]);
      if ($drush === TRUE) {
        if ($primary) {
          drush_log("Updated meeting $m_id..", 'ok');
        }
        else {
          drush_log("Removed stale addenum $m_id..", 'ok');
        }
      }
    }
  }

  $debug && error_log('MM Import - Start reset addendums on ' . count($nids_with_addendums) . ' nodes - ' . timer_read('_os2web_edoc_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());
  // Remove any addendums from nodes which hasnt been proccessed.
  foreach ($nids_with_addendums as $nid) {
    // Load the node from the local variable.
    $node = $nodes[$nid];
    $node->field_os2web_meetings_addendum = array();
    node_save($node);
    if ($drush === TRUE) {
      drush_log("Resetting $nid..", 'ok');
    }
  }
  $debug && error_log('MM Import - End of addendums - ' . timer_read('_os2web_edoc_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());
  lock_release(__FUNCTION__);
}

/**
 * This function generates the meeting xml ID, when it is missing in the XML.
 *
 * @param string $meeting_date
 *   The date of meeting.
 * @param string $committee
 *   The committee ID.
 *
 * @return int|string
 *   XML ID.
 */
function _os2web_edoc_esdh_generate_meeting_id($meeting_date, $committee) {
  // Generated meeting_id.
  $meeting_id = sprintf("%u", crc32($meeting_date . ' ' . $committee));
  if ($meeting_id > 2147483647) {
    // Mysql int out of range fix.
    $meeting_id = substr($meeting_id, 0, 9);
  }
  return $meeting_id;
}

/**
 * This function generates the ID for bullet point, if it is missing in the XML.
 *
 * @param string $committee
 *   The committee ID.
 * @param string $meeting_date
 *   The date of meeting.
 * @param string $doc_id
 *   Doc ID.
 */
function _os2web_edoc_esdh_generate_item_id($committee, $meeting_date, $doc_id) {
  $item_id = sprintf("%u", crc32($committee . ' ' . $meeting_date . ' ' . $doc_id));
  if ($item_id > 2147483647) {
    // Mysql int out of range fix.
    $item_id = substr($item_id, 0, 9);
  }
  return $item_id;
}

/**
 * This function generates the ID for committee, when it is missing in the XML.
 *
 * @param string $committee_name
 *   The name of committee.
 *
 * @return int|string
 *   Generated ID.
 */
function _os2web_edoc_esdh_generate_committee_id($committee_name) {
  $committee_id = sprintf("%u", crc32($committee_name));
  if ($committee_id > 2147483647) {
    // Mysql int out of range fix.
    $committee_id = substr($committee_id, 0, 9);
  }
  return $committee_id;
}
