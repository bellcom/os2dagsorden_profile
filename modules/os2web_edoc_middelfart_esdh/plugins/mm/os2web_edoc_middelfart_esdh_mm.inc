<?php

/**
 * @file
 * This file describes the MM plugin as implemented by eDoc.
 */
define('MM_REPUBLISH_MEETINGS', TRUE);

/**
 * Plugin definition with callbacks.
 */
$plugin = array(
  'description' => t('eDoc ESDH MM Backend'),
  'version' => 1,
  'test' => 'os2web_edoc_middelfart_esdh_mm_test',
  'get_import_list' => 'os2web_edoc_middelfart_esdh_mm_get_import_list',
  'import_meeting' => 'os2web_edoc_middelfart_esdh_mm_import_meeting',
  'post_import_process' => '_os2web_edoc_middelfart_esdh_mm_order_addenums',
);

/**
 * For plugin testing purposes.
 */
function os2web_edoc_middelfart_esdh_mm_test($stuff) {
  return 'Test successful. Recieved: ' . $stuff;
}

/**
 * Gets the Plugin ID.
 *
 * Plugin ID must be same as the filename with no exception.
 */
function os2web_edoc_middelfart_esdh_mm_get_plugin_id() {
  return 'os2web_edoc_middelfart_esdh_mm';
}

/**
 * Returns a list of meeting_ids for import.
 *
 * In our case a list of uri's of xml files.
 *
 * @return array
 *   List of URIs
 */
function os2web_edoc_middelfart_esdh_mm_get_import_list() {
  /*
   * For the eDoc service, we need to import meetings from xml files.
   * We will enumerate the list with the publication-id
   * as parsed from the dir name of the manifest.
   */
  $uris = array();
  foreach (file_scan_directory(variable_get('os2web_edoc_middelfart_esdh_mm_path', OS2WEB_EDOC_MIDDELFART_ESDH_MM_IMPORT_DIR), '/\.xml$/i', array('recurse' => FALSE, 'min_depth' => 0)) as $xml_file) {
    $meeting_date = NULL;
    $committee = NULL;

    // Loading meeting manifest.
    $real_path = drupal_realpath($xml_file->uri);
    if ($real_path !== FALSE && is_file($real_path)) {
      libxml_use_internal_errors(TRUE);
      $manifest = simplexml_load_string(file_get_contents($real_path), 'SimpleXMLElement', LIBXML_NOWARNING);
      libxml_clear_errors();
      libxml_use_internal_errors(FALSE);

      if (is_object($manifest)) {
        $meeting_path = $manifest->xpath('/Root/edoc:Notification/edoc:PathToXml');
        $meeting_path = (string) array_shift($meeting_path);

        $meeting_id = $manifest->xpath('/Root/edoc:Notification/edoc:MeetingIdentifier');
        $meeting_id = (string) array_shift($meeting_id);
        $meeting_id = _os2web_edoc_middelfart_esdh_generate_id($meeting_id);

        if(strpos($meeting_path, 'Tillægsdagsorden') !== false){
          $meeting_id = $meeting_id . '_add';
        }

        $agenda_id = $manifest->xpath('/Root/edoc:Notification/edoc:AgendaIdentifier');
        $agenda_id = (string) array_shift($agenda_id);
        $agenda_id = _os2web_edoc_middelfart_esdh_generate_id($agenda_id);

        $isPublished = $manifest->xpath('/Root/edoc:Notification/edoc:Publish');
        $isPublished = (string) array_shift($isPublished);
        $isPublished = (strcmp($isPublished, 'true') === 0) ? TRUE : FALSE;

        // Skipping if the last modified date is smaller, meaning older file
        // in this way we get only the newest agendas.
        if (isset($uris[$meeting_id])) {
          if (filemtime($real_path) < $uris[$meeting_id]['lastModified']) {
            continue;
          }
        }

        $uris[$meeting_id] = array(
          'id' => $agenda_id,
          'uri' => $xml_file->uri,
          'publish' => $isPublished,
          'lastModified' => filemtime($real_path),
          'plugin_id' => os2web_edoc_middelfart_esdh_mm_get_plugin_id(),
        );
      }
    }
  }

  return $uris;
}

/**
 * Imports a meeting, and return the import as a structured array.
 *
 * @param object $agenda
 *   Meeting XML object.
 *
 * @return array
 *   List of meeting_structures to save
 */
function os2web_edoc_middelfart_esdh_mm_import_meeting($agenda) {
  $file = $agenda['uri'];
  if ($file === FALSE) {
    _os2web_edoc_middelfart_esdh_mm_order_addenums();
  }
  $meetings = array();

  $real_path = drupal_realpath($file);
  if ($real_path !== FALSE && is_file($real_path)) {
    libxml_use_internal_errors(TRUE);
    $manifest = simplexml_load_string(file_get_contents($real_path), 'SimpleXMLElement', LIBXML_NOWARNING);
    libxml_clear_errors();
    libxml_use_internal_errors(FALSE);

    $meeting_id = $manifest->xpath('/Root/edoc:Notification/edoc:MeetingIdentifier');
    $meeting_id = (string) array_shift($meeting_id);
    $meeting_id = _os2web_edoc_middelfart_esdh_generate_id($meeting_id);

    $agenda_id = $manifest->xpath('/Root/edoc:Notification/edoc:AgendaIdentifier');
    $agenda_id = (string) array_shift($agenda_id);
    $agenda_id = _os2web_edoc_middelfart_esdh_generate_id($agenda_id);

    // This meeting must be unpublished.
    if (!$agenda['publish']) {

      $meetings[] = array(
        'publish' => FALSE,
        'meeting_id' => $meeting_id,
        'system_id' => $agenda_id,
        'meeting_sub_id' => $agenda_id,
        'path' => drupal_realpath($file),
      );
      // This meeting will be published.
    }
    else {
      // Loading agenda manifest.
      $path_to_xml = $manifest->xpath('/Root/edoc:Notification/edoc:PathToXml');
      $path_to_xml = (string) array_shift($path_to_xml);
      $path_to_xml = str_replace('\\', '/', $path_to_xml);

      $agenda_real_path = dirname($real_path) . $path_to_xml;
      $agenda_relative_path = str_replace(drupal_realpath(variable_get('os2web_edoc_middelfart_esdh_mm_path')), variable_get('os2web_edoc_middelfart_esdh_mm_path'), $agenda_real_path);

      // If agenda cannot be loaded.
      if ($real_path == FALSE || !is_file($real_path)) {
        return $meetings;
      }

      libxml_use_internal_errors(TRUE);
      $agenda_manifest = simplexml_load_string(file_get_contents($agenda_real_path), 'SimpleXMLElement', LIBXML_NOWARNING);
      libxml_clear_errors();
      libxml_use_internal_errors(FALSE);

      if ($agenda_manifest === FALSE) {
        error_log("Failed loading XML");
        foreach (libxml_get_errors() as $error) {
          error_log($error->message);
        }
      }
      if (is_object($agenda_manifest)) {
        // Collecting agendas.
        $meeting_agendas = $agenda_manifest->xpath("/Root/edoc:Committee/edoc:Meeting/edoc:Agenda");
        foreach ($meeting_agendas as $meeting_agenda) {

          // Collecting agenda data.
          $committee_xpath = $agenda_manifest->xpath('/Root/edoc:Committee/edoc:Name');
          $committee = (string) array_shift($committee_xpath);

          $meeting_date_start_xpath = $agenda_manifest->xpath('/Root/edoc:Committee/edoc:Meeting/edoc:MeetingDateTime');
          $meeting_date_start = (string) array_shift($meeting_date_start_xpath);

          $meeting_date_end_xpath = $agenda_manifest->xpath('/Root/edoc:Committee/edoc:Meeting/edoc:EndDateTime');
          $meeting_date_end = (string) array_shift($meeting_date_end_xpath);

          $meeting_type_xpath = $agenda_manifest->xpath('/Root/edoc:OutputTypeEnumName');
          $meeting_type = (string) array_shift($meeting_type_xpath);
          if (strcasecmp($meeting_type, 'Minute') === 0) {
            $meeting_type = OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_REFERAT;
          }
          else {
            $meeting_type = OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_DAGSORDEN;
          }

          $agenda_access = 'Lukket';

          $location_xpath = $agenda_manifest->xpath('/Root/edoc:Committee/edoc:Meeting/edoc:MeetingLocation');
          $location = (string) array_shift($location_xpath);

          // Collecting agenda items.
          $meeting_agendas_items = $meeting_agenda->xpath('edoc:AgendaElements');

          // Fix dates.
          $date_obj = date_create($meeting_date_start, new DateTimeZone(date_default_timezone()));
          date_timezone_set($date_obj, timezone_open('UTC'));
          $meeting_date_start = date_format($date_obj, 'Y-m-d H:i');
          $date_obj = date_create($meeting_date_end, new DateTimeZone(date_default_timezone()));
          date_timezone_set($date_obj, timezone_open('UTC'));
          $meeting_date_end = date_format($date_obj, 'Y-m-d H:i');

          // Checking if meeting is additional/tillaægsdagsorden.
          $agenda_type = $meeting_agenda->xpath('edoc:Type');
          $agenda_type = (string) array_shift($agenda_type);

          // Saving the meeting data in ESDH Provider format.
          $meeting = array(
            'system_id' => $agenda_id,
            'meeting_id' => $meeting_id,
            'meeting_sub_id' => $agenda_id,
            'committee' => $committee,
            'committee_name' => $committee,
            'committee_id' => _os2web_edoc_middelfart_esdh_generate_id($committee),
            'location' => $location,
            'title' => (stripos($agenda_type, 'tillæg') !== false) ? $agenda_type . ': ' . $committee : $committee,
            'uri' => dirname($agenda_relative_path),
            'path' => $agenda_real_path,
            'meeting_date' => $meeting_date_start,
            'meeting_date_start' => $meeting_date_start,
            'meeting_date_finish' => $meeting_date_end,
            'meeting_type' => $meeting_type,
            'type' => $meeting_type,
            'meeting_state' => $meeting_type,
            'agenda_xml' => $agenda_real_path,
            'agenda_access' => $agenda_access,
          );

          // Collect participants.
          $meeting_participants = $agenda_manifest->xpath("/Root/edoc:Committee/edoc:Meeting/edoc:MeetingParticipants/edoc:MeetingParticipant");
          foreach ($meeting_participants as $meeting_participant) {
            $participate_xpath = $meeting_participant->xpath("edoc:Participate");
            $name_xpath = $meeting_participant->xpath("edoc:Name");
            $role_in_meeting = (string) array_shift($meeting_participant->xpath("edoc:RoleInMeeting"));
            $participant_name = (string) array_shift($name_xpath);
            if ((string) array_shift($participate_xpath) === 'true') {
              $meeting['participants'][] = $participant_name;
              if ($role_in_meeting === 'Referent') {
                $meeting['referent'][] = $participant_name;
              }
            }
            else {
              $meeting['participants_cancellation'][] = $participant_name;
            }
          }

          // Error logging to watchdog if the agenda item list is empty.
          if (empty($meeting_agendas_items)) {
            watchdog('eDoc MM', 'Empty list of import items in %file.', array('%file' => $file), WATCHDOG_WARNING);
          }

          foreach ($meeting_agendas_items as $meeting_agenda_item) {
            // Importing each agenda item through the ESDH provider module.
            $imported = _os2web_edoc_middelfart_esdh_mm_import_meeting_agenda($meeting, $meeting_agenda_item);
            if (is_array($imported)) {
              $imported['publish'] = $agenda['publish'];
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
function _os2web_edoc_middelfart_esdh_mm_import_meeting_agenda(&$meeting, SimpleXMLElement $agenda_xml) {
  // Check if the committee is allowed to publish.
  $publish_committee = array_map('trim', explode(',', variable_get('os2web_edoc_middelfart_esdh_approved_committees', OS2WEB_EDOC_MIDDELFART_ESDH_MM_DEFAULT_APPROVED)));
  if ($meeting['committee'] === '' || !in_array($meeting['committee'], $publish_committee)) {
    watchdog('eDoc MM', 'Ignored agenda from "%committee" .', array('%committee' => $meeting['committee']));
    return FALSE;
  }

  $meeting['items'] = array();

  $agenda = $meeting;
  // Import all agenda items.
  $agenda_bullet = $agenda_xml;
  foreach ($agenda_bullet->xpath('edoc:AgendaElement') as $agenda_item) {
    if (($item = _os2web_edoc_middelfart_esdh_mm_import_meeting_agenda_item($meeting, $agenda_item)) !== FALSE) {
      $agenda['items'][] = $item;
    }
  }

  // Apply defaults, just in case we missed something.
  $agenda += os2web_esdh_provider_default_meeting();
  return $agenda;
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
function _os2web_edoc_middelfart_esdh_mm_import_meeting_agenda_item($meeting, SimpleXMLElement $xml) {
  $xml_item = array();
  $serialno_xpath = $xml->xpath('edoc:HandlingItem/edoc:SerialNo');
  $xml_item['number'] = (int) array_shift($serialno_xpath);
  $handlingPlan_xpath = $xml->xpath('edoc:HandlingItem/edoc:HandlingPlan');
  $xml_handlingPlans = array_shift($handlingPlan_xpath);
  $casePresentations_xpath = $xml->xpath('edoc:HandlingItem/edoc:CasePresentations');
  $xml_item_bullets = array_shift($casePresentations_xpath);
  $attachment_xpath = $xml->xpath('edoc:HandlingItem/edoc:Attachments');
  $xml_item_enclosures = array_shift($attachment_xpath);

  $itemTitle_xpath = $xml->xpath('edoc:HandlingItem/edoc:Title');
  $xml_item['name'] = (string) array_shift($itemTitle_xpath);

  $identifier_xpath = $xml->xpath('edoc:HandlingItem/edoc:Identifier');
  $xml_item['documentid'] = (string) array_shift($identifier_xpath);
  $accessIsPublic_xpath = $xml->xpath('edoc:AccessIsPublic');
  $xml_item['access'] = (string) array_shift($accessIsPublic_xpath);
  $caseNumber_xpath = $xml->xpath('edoc:HandlingItem/edoc:CaseNumber');
  $xml_item['case_nr'] = (string) array_shift($caseNumber_xpath);
  $caseIdentifier_xpath = $xml->xpath('edoc:HandlingItem/edoc:CaseIdentifier');
  $xml_item['case_id'] = (string) array_shift($caseIdentifier_xpath);

  $ruleOfSpeaking_xpath = $xml->xpath('edoc:RuleOfSpeaking');
  $xml_item['comname'] = (string) array_shift($ruleOfSpeaking_xpath);

  $behandlingsplan_place =  explode(',', variable_get('os2web_edoc_middelfart_esdh_bp_before_behandlingsplan', 'Høring,Økonomi,Sagsbeskrivelse,Forvaltningen foreslår'));

  if ($xml_item['access'] !== 'true') {
    $xml_item['access'] = 'Lukket';
  }

  $item = array(
    'id' => $xml_item['number'],
    // Generated id - because there is not unique ID for the bullet point.
    'sysid' => _os2web_edoc_middelfart_esdh_generate_id($xml_item['documentid']),
    'case_nr' => $xml_item['case_nr'],
    'case_id' => $xml_item['case_id'],
    'title' => 'Punkt ' . $xml_item['number'] . ' ' . $xml_item['name'],
    'access' => $xml_item['access'],
    'comname' => isset($xml_item['comname']) ? $xml_item['comname'] : NULL,
    'bullets' => array(),
    'enclosures' => array(),
  );

  if ($item['access'] !== 'Lukket' || variable_get('os2web_edoc_middelfart_esdh_import_closed_agenda', FALSE)) {
     // Process behandlingplans.
    $behandlingplan_meetings = array();
    $behandlingplan_decisions = array();

    foreach ($xml_handlingPlans->xpath('edoc:Handling') as $key => $behandlingplan_xml) {
      $title = (string) array_shift($behandlingplan_xml->xpath('edoc:CommitteeName'));
      $date = (string) array_shift($behandlingplan_xml->xpath('edoc:MeetingDate'));
      $date = format_date(strtotime($date), 'os2dagsorden_long_date');
      $decision = (string) array_shift($behandlingplan_xml->xpath('edoc:Decision'));
      $excluded_decisions = explode(',', variable_get('os2web_edoc_middelfart_esdh_behandlingsplan_decision_exclude', 'Beslutning ikke frigivet'));
      $behandlingplan_meetings[$key] =  $title . ' ' . t('den') . ' ' . $date;
      if (!in_array($decision, $excluded_decisions)) {
        $behandlingplan_decisions[$key] = (string) array_shift($behandlingplan_xml->xpath('edoc:Decision'));
      }
    }
    if (!empty($behandlingplan_meetings)) {
      $bullet_body = '<strong>' . t('Behandlingsplan') . ':</strong><br/>';
      $bullet_body .= implode('<br/>', $behandlingplan_meetings);
      if (!empty($behandlingplan_decisions)) {
        $bullet_body .= '<br/><br/>';
        foreach ($behandlingplan_decisions as $key => $decision) {
          if (!empty($decision)) {
            $bullet_body .= '<strong>' . $behandlingplan_meetings[$key] . '</strong><br/>';
            $bullet_body .= $decision . '<br/>';
          }
        }
      }
      $bullet_title = t('Behandlingsplan');
      $bullet_id = _os2web_edoc_middelfart_esdh_generate_id($bullet_title);
      $behandlingsplan_item = array(
          'title' => $bullet_title,
          'body' => _os2web_edoc_esdh_mm_improve_body_html($bullet_body, $meeting['uri']),
          'sysid' => $bullet_id,
        );
    }
    $bpa_indexes = array();
    // Process bullets.
    foreach ($xml_item_bullets->xpath('edoc:CasePresentation') as $bullet_xml) {
      $title_xpath = $bullet_xml->xpath('edoc:Title');
      $bullet_title = (string) array_shift($title_xpath);
      $content_xpath = $bullet_xml->xpath('edoc:Content');
      $bullet_body = (string) array_shift($content_xpath);
      $bullet_id = _os2web_edoc_middelfart_esdh_generate_id($bullet_title);

      if (!empty($bullet_title) && !empty($bullet_body)) {
        $item['bullets'][] = array(
          'title' => (string) $bullet_title,
          'body' => _os2web_edoc_esdh_mm_improve_body_html((string) $bullet_body, $meeting['uri']),
          'sysid' => $bullet_id,
        );
      $key = array_search($bullet_title, $behandlingsplan_place);
        if ($key !== FALSE) {
           $bpa_indexes[$key] = count($item['bullets']);
        }
      }
    }
    //dpm($bpa_indexes);
    if (!empty($bpa_indexes)) {
      ksort($bpa_indexes);
      $index = reset($bpa_indexes);
      array_splice($item['bullets'], $index, 0, array($behandlingsplan_item));
    }

    // Process enclosures.
    foreach ($xml_item_enclosures->xpath('edoc:Attachment') as $bullet_xml) {
      $enclosure_item = array();
      $title_xpath = $bullet_xml->xpath('edoc:Title');
      $enclosure_item['title'] = (string) array_shift($title_xpath);
      $accessName_xpath = $bullet_xml->xpath('edoc:AccessEnumName');
      $enclosure_item['access'] = ((string) array_shift($accessName_xpath) == 'Closed') ? FALSE : TRUE;
      $identifier_xpath = $bullet_xml->xpath('edoc:Identifier');
      $enclosure_item['sysid'] = _os2web_edoc_middelfart_esdh_generate_id((string) array_shift($identifier_xpath));
      $link_xpath = $bullet_xml->xpath('edoc:Link');
      $enclosure_item['uri'] = (string) array_shift($link_xpath);

      $item['enclosures'][] = array(
        'uri' => implode('/', array(
          trim($meeting['uri']),
          str_replace('\\', '/', $enclosure_item['uri']),
        )),
        'title' => trim($enclosure_item['title']),
        'sysid' => $enclosure_item['sysid'],
        'access' => $enclosure_item['access'],
        'filetype' => '.pdf',
        'type' => 'Bilag',
      );
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
function _os2web_edoc_middelfart_esdh_similar_file_exists($filename) {
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
function _os2web_edoc_middelfart_esdh_mm_order_addenums($drush = FALSE) {
  if (!lock_acquire(__FUNCTION__)) {
    return;
  }

  // Debug bit set in os2web_esdh_provider settings.
  $debug = variable_get('os2web_esdh_provider_debug_info');
  $debug && timer_start('_os2web_edoc_middelfart_esdh_mm_order_addenums') && error_log('MM Import - Start of addendums - ' . timer_read('_os2web_edoc_middelfart_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());

  // Find all NIDs that belong to this plugin_id.
  $meeting_nids = db_select('os2web_esdh_provider_import', 'o')
    ->fields('o', array('nid'))
    ->condition('plugin_id', os2web_edoc_middelfart_esdh_mm_get_plugin_id())
    ->execute()
    ->fetchCol();

  // Find all meetings and gather meeting and agenda_ids.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node', '=')
    ->propertyCondition('type', 'os2web_meetings_meeting', '=')
    ->propertyCondition('nid', $meeting_nids, 'IN');
  $nodes_result = $query->execute();

  if (!isset($nodes_result['node'])) {
    return;
  }
  $nodes = $nodes_result['node'];
  $nodes = node_load_multiple(array_keys($nodes));

  // Meetings as: $meetings[<meeting_id>][<system_id>][<node_id>] .
  $meetings = array();

  $nids_with_addendums = array();

  $debug && error_log('MM Import - Retrieve nodes with addendums from ' . count($nodes) . ' nodes - ' . timer_read('_os2web_edoc_middelfart_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());
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

  $debug && error_log('MM Import - Update addendums on ' . count($meetings) . ' nodes - ' . timer_read('_os2web_edoc_middelfart_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());

  // Run through the meeting array, to determine the addendums.
  foreach ($meetings as $m_id => $meeting_versions) {
    // Find primary node
    $primary = NULL;
    foreach ($meeting_versions as $meeting_version_key => $meeting_version) {
      if ((stripos($nodes[$meeting_version]->title, 'tillæg') !== 0)) {
        $primary = $nodes[$meeting_version];

        // Removing primary node from our list.
        unset($meeting_versions[$meeting_version_key]);
        break;
      }
    }
    if ($primary) {
      // Resetting primary node field_os2web_meetings_addendum field, if present.
      $addenum_to = field_get_items('node', $primary, 'field_os2web_meetings_addendum');
      if (!empty($addenum_to[0]['nid'])) {
        $primary->field_os2web_meetings_addendum[LANGUAGE_NONE] = array();
        node_save($primary);
      }
    }
    // Updating additional/tillægsdagsorden with our primary meeting reference.
    foreach ($meeting_versions as $meeting_version_key => $meeting_version) {
      $additional_meeting = $nodes[$meeting_version];

      if (!isset($additional_meeting->field_os2web_meetings_addendum[LANGUAGE_NONE]) || $additional_meeting->field_os2web_meetings_addendum[LANGUAGE_NONE][0]['nid'] !== $primary->nid) {
        $additional_meeting->field_os2web_meetings_addendum[LANGUAGE_NONE][0]['nid'] = $primary->nid;
        node_save($additional_meeting);
      }

      // Remove node from final nids_with_addendums.
      unset($nids_with_addendums[$additional_meeting->nid]);
    }
  }

  // Remove any addendums from nodes which hasnt been proccessed.
  $debug && error_log('MM Import - Start reset addendums on ' . count($nids_with_addendums) . ' nodes - ' . timer_read('_os2web_edoc_middelfart_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());
  foreach ($nids_with_addendums as $nid) {
    // Load the node from the local variable.
    $node = $nodes[$nid];
    $node->field_os2web_meetings_addendum = array();
    node_save($node);
  }

  $debug && error_log('MM Import - End of addendums - ' . timer_read('_os2web_edoc_middelfart_esdh_mm_order_addenums') . 'ms - ' . memory_get_usage());

  lock_release(__FUNCTION__);
}

/**
 * This function generates the INT value from a string ID using crc32 function.
 *
 * @param string $string
 *   String ID.
 *
 * @return int|string
 *   Generates the INT value.
 */
function _os2web_edoc_middelfart_esdh_generate_id($string) {
  // Generated meeting_id.
  $meeting_id = sprintf("%u", crc32($string));
  if ($meeting_id > 2147483647) {
    // Mysql int out of range fix.
    $meeting_id = substr($meeting_id, 0, 9);
  }
  return $meeting_id;
}

/**
 * Improve the body HTML by substituting img references,.
 *
 * @param string $data
 *   String - the body text.
 * @param string $meeting_uri
 *   String - the uri of the meeting folder.
 *
 * @return mixed
 *   Improved string.
 */
function _os2web_edoc_esdh_mm_improve_body_html($data, $meeting_uri) {
  $doc = new DOMDocument();
  $doc->loadHTML($data);
  $elements = $doc->getElementsByTagName('img');
  if ($elements->length > 0) {
    foreach ($elements as $element) {
      $file = $element->getAttribute('src');
      if (strpos($file, 'http://') === FALSE  && preg_match('/^data:image\/(\w+);base64,/', $file) == FALSE ) {
        $file = str_replace('"', '', $file);
        $uri_parts = preg_split('/(\/|\\\\)/m', $file);
        // Saving the file location information.
        $filename = trim(array_pop($uri_parts));
        $folder_name = array_pop($uri_parts);

        $file_realpath = drupal_realpath($meeting_uri . '/' .
          implode('/', array(
            $folder_name,
            $filename,
          )));

        $copy_destination = 'public://edoc_images/' . $folder_name;

        watchdog('acadre MM', 'Copying %from to %to', array(
          '%from' => $file_realpath,
          '%to' => $copy_destination . '/' . $filename,
        ), WATCHDOG_DEBUG);

        // Since drupal can end up with different URL because of
        // FILE_EXISTS_RENAME flag, we need to catch it.
        $destination_file_uri = file_unmanaged_copy($file_realpath, $copy_destination);

        // Modify url.
        $new_url = file_create_url($destination_file_uri);
        $data = str_replace($file, $new_url, $data);
      }
    }
  }
  return $data;
}
