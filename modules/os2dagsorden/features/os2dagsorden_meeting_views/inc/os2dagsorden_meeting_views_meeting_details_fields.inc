<?php

/**
 * @file
 * Code for the OS2Web Meeting Views - meeting details view.
 */

/**
 * Implements hook_breadcrumbs.
 */
function os2dagsorden_meeting_views_meeting_details_breadcrumb() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $meeting_id = $q[1];
  $breadcrumb[] = l(t('Forsiden'), $base_url);
  $breadcrumb[] .= '<span class="breadcrumb-active">Mødedetaljer</span>';

  drupal_set_breadcrumb($breadcrumb);

  // User has no access to agenda.
  $meeting = node_load($meeting_id);
  $meeting_full_access_granted = os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting);

  if ($meeting_full_access_granted) {
    // Meeting all notes.
    echo '<div class="controls-right breadcrumb-level">';
    if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
      $meeting = node_load($meeting_id);
      $committee_tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
      if (os2dagsorden_access_helper_check_user_can_create_meeting_in_committee($committee_tid) === TRUE) {
        echo '<a href="' . $base_url . '/agenda/meeting/' . $meeting_id . '" class="form-submit" rel="lightframe[|width:800px; height:500px; scrolling: auto;]">' . t('Edit') . '</a>';

      }
    }
    echo '<a target="_blank" href="' . $base_url . '?q=meeting/' . $meeting_id . '/notes" class="form-submit">' .
        variable_get('os2dagsorden_meeting_show_notes_button', 'Mødernoter') . '</a>';
    echo '</div>';

  }
}

/**
 * Prints agenda edit button.
 */
function os2dagsorden_meeting_views_meeting_details_agenda_and_edit_button() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $meeting_id = $q[1];

  if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
    $meeting = node_load($meeting_id);
    $committee_tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
    if (os2dagsorden_access_helper_check_user_can_create_meeting_in_committee($committee_tid) === TRUE) {

      if (os2dagsorden_access_helper_check_user_can_delete_meeting_content($committee_tid) === TRUE) {
        print '<div class="controls-right">';
        if (variable_get('os2dagsorden_external_committee_anonymize_button_behavior', 'anonymize') === 'delete') {
          print '<a href="' . $base_url . '/agenda/' . $meeting_id . '/delete" class="form-submit">' . t('Delete meeting') . '</a>';
        }
        else {
          print '<a href="' . $base_url . '/agenda/' . $meeting_id . '/clear" class="form-submit">' . t('Anonymize') . '</a>';
        }
        print '</div>';
      }
      print '<div class="controls-right">';
      print '</div>';
      print '<div class="clearfix"></div>';
    }
  }
}

/**
 * Prints agenda download button.
 */
function os2dagsorden_meeting_views_meeting_details_description_download() {
  // Disallowing PDF download.
  if (!os2dagsorden_access_helper_check_pdf_download_enabled()) {
    return '';
  }

  global $base_url;
  $q = explode('/', $_GET['q']);
  $meeting_id = $q[1];

  // User has no access to agenda.
  $meeting = node_load($meeting_id);
  if (!node_access('view', $meeting)) {
    return;
  }
  $meeting_full_access_granted = os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting);
  // Descriptions full doc.
  $has_full_doc = FALSE;
  $has_full_doc_attachments = FALSE;
  $has_full_doc_html = FALSE;
  $has_full_doc_attachments_html = FALSE;
  $has_add_doc = FALSE;
  $has_add_doc_attachments = FALSE;
  $pdf_documents_lists_order = array (
    'full_doc',
    'full_doc_html',
    'att_doc',
    'att_doc_html',
    'full_doc_c',
    'full_doc_c_html',
    'att_doc_c',
    'att_doc_c_html',
    'full_doc_p',
    'att_doc_p'
  );
  $html_documents_list = array (
    'full_doc_html' => variable_get('os2dagsorden_general_bullet_full_doc_title', t('Åben samlet dokument')),
    'att_doc_html' => variable_get('os2dagsorden_general_bullet_full_doc_with_attach_title', t('Åben samlet dokument med bilag')),
    'full_doc_c_html' => variable_get('os2dagsorden_general_bullet_full_doc_title', t('Åben samlet dokument')) . '(Lukket)',
    'att_doc_c_html' => variable_get('os2dagsorden_general_bullet_full_doc_with_attach_title', t('Åben samlet dokument med bilag')) . '(Lukket)'
  );

  echo '<div class="controls-right dropdown">';
  echo '<a class="dropbtn form-submit" type="button" data-toggle="dropdown">'
    . variable_get('os2dagsorden_meeting_description_download_dropdown', 'Hent dokumenter')
    . ' &#9660;</a>';
  echo '<div class="dropdown-content">';

  if (os2dagsorden_access_helper_check_pdf_download_enabled()) {
    $pdf_documents_links = array();
    // Closed version - if not empty.
    $user_can_see_personal_agendas = os2dagsorden_access_helper_check_user_can_see_personal_agenda($meeting->nid);
    $user_can_see_closed_agendas = os2dagsorden_access_helper_check_user_can_see_closed_agenda($meeting->nid);
    if (module_exists('os2web_esdh_provider')) {
      $agenda_access = _os2web_esdh_provider_get_agenda_access($meeting->nid, $meeting->field_os2web_meetings_sub_id['und'][0]['value']);
    }

   if ($user_can_see_personal_agendas) {
      if (!empty($meeting->field_os2web_meetings_full_doc_p['und'][0]['uri'])) {
        $pdf_documents_links['full_doc_p'] = '<a target="_blank" href="' . file_create_url($meeting->field_os2web_meetings_full_doc_p['und'][0]['uri']) .
          '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_download_title', 'Samlet dokument') . ' (person)</a>';
        $has_full_doc = TRUE;
      }
    }

    if ($user_can_see_closed_agendas && ((!isset($agenda_access) || $agenda_access === 0) || variable_get('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', FALSE))) {
      if (!empty($meeting->field_os2web_meetings_full_doc_c['und'][0]['uri'])) {
        $pdf_documents_links['full_doc_c'] = '<a target="_blank" href="' . file_create_url($meeting->field_os2web_meetings_full_doc_c['und'][0]['uri']) .
          '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_download_title', 'Samlet dokument') . ' (lukket)</a>';
        $has_full_doc = TRUE;
      }
    }
    if ($meeting_full_access_granted) {
      // Only if closed/personal has not been printed yet.
      if (!$has_full_doc || variable_get('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', FALSE)) {
        // Opened version - if not empty.
        if (!empty($meeting->field_os2web_meetings_full_doc['und'][0]['uri'])) {
          $pdf_documents_links['full_doc'] = '<a target="_blank" href="' . file_create_url($meeting->field_os2web_meetings_full_doc['und'][0]['uri']) .
            '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_download_title', 'Samlet dokument') . '</a>';
          $has_full_doc = TRUE;
        }
      }
    }
    // Descriptions full doc END.
    // descriptions full doc with attachments
    // personal version - if not empty.
    if ($user_can_see_personal_agendas) {
      if (!empty($meeting->field_os2web_meetings_att_doc_p['und'][0]['uri'])) {
        $pdf_documents_links['att_doc_p'] = '<a target="_blank" href="' . file_create_url($meeting->field_os2web_meetings_att_doc_p['und'][0]['uri']) .
          '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_with_att_download_title', 'Samlet dokument med bilag') . ' (person)</a>';
        $has_full_doc_attachments = TRUE;
      }
    }

    // Closed version - if not empty.
    if ($user_can_see_closed_agendas && ((!isset($agenda_access) || $agenda_access === 0) || variable_get('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', FALSE))) {
      if (!empty($meeting->field_os2web_meetings_att_doc_c['und'][0]['uri'])) {
        $pdf_documents_links['att_doc_c'] = '<a target="_blank" href="' . file_create_url($meeting->field_os2web_meetings_att_doc_c['und'][0]['uri']) .
          '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_with_att_download_title', 'Samlet dokument med bilag') . ' (lukket)</a>';
        $has_full_doc_attachments = TRUE;
      }
    }
    if ($meeting_full_access_granted) {
      // Only if closed/personal has not been printed yet.
      if (!$has_full_doc_attachments || variable_get('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', FALSE)) {
        // Opened version - if not empty.
        if (!empty($meeting->field_os2web_meetings_att_doc['und'][0]['uri'])) {
          $pdf_documents_links['att_doc'] = '<a target="_blank" href="' . file_create_url($meeting->field_os2web_meetings_att_doc['und'][0]['uri']) .
            '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_with_att_download_title', 'Samlet dokument med bilag') . '</a>';
          $has_full_doc_attachments = TRUE;
        }
      }
    }


    if ($bullet_point = _os2dagsorden_access_helper_get_genaral_bullet_point($meeting->nid)) {
      $html_documents = array();
      foreach($bullet_point->field_os2web_meetings_attach['und'] as $attachnemt_nid) {
        $attachment =  node_load($attachnemt_nid['target_id']);
        if (($key = array_search($attachment->title, $html_documents_list)) && !empty($attachment->field_os2web_meetings_bpa_html['und'][0]['uri'])) {
          $html_documents[$key] =  '<a target="_blank" href="/meeting/' . $meeting->nid . '/bullet-point/' . $bullet_point->nid . '/bullet-point-attachment/' . $attachnemt_nid['target_id'] .
            '" class="btn-group">' . $attachment->title . '</a>';
        }
      }
      if ($user_can_see_closed_agendas && ((!isset($agenda_access) || $agenda_access === 0) || variable_get('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', FALSE))) {
        if (isset($html_documents['full_doc_c_html'])) {
          $pdf_documents_links['full_doc_html'] = $html_documents['full_doc_c_html'];
          $has_full_doc_html = TRUE;
        }
        if (isset($html_documents['att_doc_c_html'])) {
          $pdf_documents_links['att_doc_html'] = $html_documents['att_doc_c_html'];
          $has_full_doc_attachments_html = TRUE;
        }
      }
      if ($meeting_full_access_granted) {
        // Only if closed/personal has not been printed yet.
        if (!$has_full_doc_html ) {
          // Opened version - if not empty.
          if (isset($html_documents['full_doc_html'])) {
            $pdf_documents_links['full_doc_html'] = $html_documents['full_doc_html'];
            $has_full_doc_html = TRUE;
          }
        }
        if (!$has_full_doc_attachments_html) {
          // Opened version - if not empty.
          if (isset($html_documents['att_doc_html'])) {
            $pdf_documents_links['att_doc_html'] = $html_documents['att_doc_html'];
            $has_full_doc_attachments_html = TRUE;
          }
        }
      }
    }
    foreach($pdf_documents_lists_order as $doc_type) {
      if (isset($pdf_documents_links[$doc_type])) {
        echo $pdf_documents_links[$doc_type];
      }
    }
    // Descriptions full doc with attachments END
    // descriptions (additional)
    if (variable_get('os2dagsorden_meeting_view_settings_not_attach_addtional_agenda', FALSE) == FALSE) {
      $additional_descriptions = array();
      if (!empty($meeting->field_os2web_meetings_add_doc_p['und'])) {
        foreach ($meeting->field_os2web_meetings_add_doc_p['und'] as $file) {
          $additional_descriptions[$file['description']]['personal'] = $file;
        }
      }
      if (!empty($meeting->field_os2web_meetings_add_doc_c['und'])) {
        foreach ($meeting->field_os2web_meetings_add_doc_c['und'] as $file) {
          $additional_descriptions[$file['description']]['closed'] = $file;
        }
      }
      if (!empty($meeting->field_os2web_meetings_add_doc['und'])) {
        foreach ($meeting->field_os2web_meetings_add_doc['und'] as $file) {
          $additional_descriptions[$file['description']]['opened'] = $file;
        }
      }

      $additional_descriptions_with_att = array();

      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'node')
        ->fieldCondition('field_os2web_meetings_addendum', 'nid', $meeting_id, '=');
      $result = $query->execute();
      if (isset($result['node'])) {
        $additional_meetings = array_keys($result['node']);

        foreach ($additional_meetings as $additional_meeting_id) {
          $additional_meeting = node_load($additional_meeting_id);

          // Plain.
          if (!empty($additional_meeting->field_os2web_meetings_full_doc['und'][0]['uri'])) {
            $additional_descriptions[]['opened'] = $additional_meeting->field_os2web_meetings_full_doc['und'][0];
          }
          if (!empty($additional_meeting->field_os2web_meetings_full_doc_c['und'][0]['uri'])) {
            $additional_descriptions[]['closed'] = $additional_meeting->field_os2web_meetings_full_doc_c['und'][0];
          }
          if (!empty($additional_meeting->field_os2web_meetings_full_doc_p['und'][0]['uri'])) {
            $additional_descriptions[]['personal'] = $additional_meeting->field_os2web_meetings_full_doc_p['und'][0];
          }

          // With attachments.
          if (!empty($additional_meeting->field_os2web_meetings_att_doc['und'][0]['uri'])) {
            $additional_descriptions_with_att[]['opened'] = $additional_meeting->field_os2web_meetings_att_doc['und'][0];
          }
          if (!empty($additional_meeting->field_os2web_meetings_att_doc_c['und'][0]['uri'])) {
            $additional_descriptions_with_att[]['closed'] = $additional_meeting->field_os2web_meetings_att_doc_c['und'][0];
          }
          if (!empty($additional_meeting->field_os2web_meetings_att_doc_p['und'][0]['uri'])) {
            $additional_descriptions_with_att[]['personal'] = $additional_meeting->field_os2web_meetings_att_doc_p['und'][0];
          }
        }
      }

      ksort($additional_descriptions);
      ksort($additional_descriptions_with_att);
      $additional_descriptions_pdf_files = array();
      foreach ($additional_descriptions as $add_desc) {
        // Personal version additional - if not empty.
        if ($user_can_see_personal_agendas) {
          if (!empty($add_desc['personal'])) {
            $additional_descriptions_pdf_files['full_doc_p'][] = '<a target="_blank" href="' . file_create_url($add_desc['personal']['uri']) .
              '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_download_title_additional', 'Tillægsdokument') . ' (person)</a>';
            $has_add_doc = TRUE;
          }
        }
        // Closed version additional - if not empty.
        if ($user_can_see_closed_agendas && (!isset($agenda_access) || $agenda_access === 0)) {
          if (!empty($add_desc['closed'])) {
            $additional_descriptions_pdf_files['full_doc_c'][] = '<a target="_blank" href="' . file_create_url($add_desc['closed']['uri']) .
              '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_download_title_additional', 'Tillægsdokument') . ' (lukket)</a>';
            $has_add_doc = TRUE;
          }
        }
        if ($meeting_full_access_granted) {
          // Only if closed/personal has not been printed yet.
          if (!$has_add_doc) {
            // Opened version additional - if not empty.
            if (!empty($add_desc['opened'])) {
              $additional_descriptions_pdf_files['full_doc'][] = '<a target="_blank" href="' . file_create_url($add_desc['opened']['uri']) .
                '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_download_title_additional', 'Tillægsdokument') . '</a>';
              $has_add_doc = TRUE;
            }
          }
        }
      }

      foreach ($additional_descriptions_with_att as $add_desc) {
        // Personal version additional - if not empty.
        if ($user_can_see_personal_agendas) {
          if (!empty($add_desc['personal'])) {
            $additional_descriptions_pdf_files['att_doc_p'][] = '<a target="_blank" href="' . file_create_url($add_desc['personal']['uri']) .
              '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_with_att_download_title_additional', 'Tillægsdokument med bilag') . ' (person)</a>';
            $has_add_doc_attachments = TRUE;
          }
        }
        // Closed version additional - if not empty.
        if ($user_can_see_closed_agendas) {
          if (!empty($add_desc['closed'])) {
            $additional_descriptions_pdf_files['att_doc_c'][] = '<a target="_blank" href="' . file_create_url($add_desc['closed']['uri']) .
              '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_with_att_download_title_additional', 'Tillægsdokument med bilag') . ' (lukket)</a>';
            $has_add_doc_attachments = TRUE;
          }
        }
        if ($meeting_full_access_granted) {
          // Only if closed/personal has not been printed yet.
          if (!$has_add_doc_attachments) {
            // Opened version additional - if not empty.
            if (!empty($add_desc['opened'])) {
              $additional_descriptions_pdf_files['att_doc'][] = '<a target="_blank" href="' . file_create_url($add_desc['opened']['uri']) .
                '" class="btn-group">' . variable_get('os2dagsorden_meeting_description_with_att_download_title_additional', 'Tillægsdokument med bilag') . '</a>';
              $has_add_doc_attachments = TRUE;
            }
          }
        }
      }

      foreach($pdf_documents_lists_order as $doc_type) {
        if (isset($additional_descriptions_pdf_files[$doc_type]) && is_array($additional_descriptions_pdf_files[$doc_type])) {
          foreach($additional_descriptions_pdf_files[$doc_type] as $pdf_doc) {
            echo $pdf_doc;
          }
        }
      }
    }

    // Generated PDF's.
    if (module_exists('os2web_print_send_to_friend')) {
      if (variable_get('os2dagsorden_show_generate_pdf_link', TRUE)) {
        // Only if has not been printed yet.
        if (!$has_full_doc) {
          echo '<a target="_blank" href="' . $base_url . '/meeting/' . $meeting_id . '/pdf">' . variable_get('os2dagsorden_meeting_description_generate_pdf_title', 'Samlet dokument i PDF') . '</a>';
          $has_description = TRUE;
        }
      }
      if ($meeting_full_access_granted) {
        if (variable_get('os2dagsorden_show_generate_pdf_link_full', TRUE)) {
          // Only if has not been printed yet.
          if (!$has_full_doc_attachments) {
            echo '<a target="_blank" href="' . $base_url . '/meeting/' . $meeting_id . '/pdf_full">' . variable_get('os2dagsorden_meeting_description_generate_pdf_full_title', 'Samlet dokument med bilag i PDF') . '</a>';
            $has_full_doc_attachments = TRUE;
          }
        }
      }
    }
  }

  // Print that it does not have anything.
  if (!$has_full_doc && !$has_full_doc_attachments && !$has_add_doc & !$has_add_doc_attachments) {
    echo '<p>Ingen</p>';
  }

  // class=dropdown-content.
  echo '</div>';
  // class=dropdown.
  echo '</div>';
  echo '<div class="clearfix"></div>';
  // Descriptions (additional) END.
}

/**
 * Prints admin buttons.
 */
function os2dagsorden_meeting_views_meeting_details_admin_controls() {
  if (user_access('administer os2web')) {
    global $base_url;
    $q = explode('/', $_GET['q']);
    $meeting_id = $q[1];

    if (variable_get('os2dagsorden_show_meeting_unpublish_button', TRUE)) {
      // Edit button.
      echo '<div class="controls-left">';
      echo '<a href="' . $base_url . '?q=node/' . $meeting_id . '/edit" class="form-submit">Afpublicer møde</a>';
      echo '</div>';
    }
    if (variable_get('os2dagsorden_show_import_form_button', TRUE)) {
      // Import form.
      echo '<div class="controls-left">';
      echo '<a href="' . $base_url . '?q=admin/config/os2web/esdh_provider" class="form-submit">Import form</a>';
      echo '</div>';
    }
  }
}

/**
 * Prints meeting speaker paper indicator.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_indicator_meeting_has_sp($meeting_id) {
  os2dagsorden_meeting_views_common_meeting_has_sp($meeting_id);
}

/**
 * Prints meeting notes indicator.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_indicator_meeting_has_notes($meeting_id) {
  os2dagsorden_meeting_views_common_meeting_has_notes($meeting_id);
}

/**
 * Prints meeting end date.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_enddate($meeting_id) {
  if (variable_get('os2dagsorden_show_meeting_enddate', TRUE)) {
    $meeting = node_load($meeting_id);
    $date = field_get_items('node', $meeting, 'field_os2web_meetings_date');
    print format_date(strtotime($date[0]["value2"] . 'UTC'), $type = 'short', '');
  }
}

/**
 * Prints meeting committee.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_committee($meeting_id) {
  if (variable_get('os2dagsorden_show_meeting_committee', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_committee($meeting_id);
  }
}

/**
 * Prints meeting type.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_type($meeting_id) {
  if (variable_get('os2dagsorden_show_meeting_type', TRUE)) {
    $meeting = node_load($meeting_id);
    $meeting_type = field_get_items('node', $meeting, 'field_os2web_meetings_type');
    $options = field_info_field('field_os2web_meetings_type');
    $addendum_to = field_get_items('node', $meeting, 'field_os2web_meetings_addendum');
    if (!empty($addendum_to[0]['nid']) && variable_get('os2dagsorden_meeting_view_settings_not_attach_addtional_agenda', FALSE)) {
      print (substr($options['settings']['allowed_values'][$meeting_type[0]['value']], -1) === '+') ? 'Tillægsdagsorden+' : 'Tillægsdagsorden';
    }
    else {
    print $options['settings']['allowed_values'][$meeting_type[0]['value']];
    }
  }
}

/**
 * Retuns of comments field must be printed.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_meeting_views_meeting_details_comments_access() {
  if (variable_get('os2dagsorden_show_comments', FALSE)) {
    $q = $_GET['q'];
    $q = explode('/', $q);
    $meeting_id = $q[1];
    $meeting = node_load($meeting_id);
    if (!empty($meeting->field_os2web_meetings_comments['und'][0]['value'])) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Retuns of special conditions field must be printed.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_meeting_views_meeting_details_special_conditions_access() {
  if (variable_get('os2dagsorden_show_comments', FALSE)) {
    $q = $_GET['q'];
    $q = explode('/', $q);
    $meeting_id = $q[1];
    $meeting = node_load($meeting_id);
    if (!empty($meeting->field_os2web_meetings_conditions['und'][0]['value'])) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Print field participants.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_participants_deltog($meeting_id) {
  if (variable_get('os2dagsorden_hide_meeting_details', FALSE) == FALSE) {
    $meeting = node_load($meeting_id);
    $participants = array();
    $internal_participants = field_get_items('node', $meeting, 'field_os2web_meetings_partic_int');
    $external_participants = field_get_items('node', $meeting, 'field_os2web_meetings_partic_ext');
    $invisible = field_get_items('node', $meeting, 'field_os2web_meetings_partic_inv');
    if ($internal_participants || $external_participants) {
      $visible_participants = array();
      $invisible_participants = array();
      if ($internal_participants) {
        foreach ($internal_participants as $participant) {
          $visible_participants[$participant['uid']] = $participant['uid'];
        }
      }
      if ($external_participants) {
        foreach ($external_participants as $participant) {
          $visible_participants[$participant['uid']] = $participant['uid'];
        }
      }
      // Hide invisible participants.
      if ($invisible) {
        foreach ($invisible as $participant) {
          unset($visible_participants[$participant['uid']]);
        }
      }
      $users = user_load_multiple($visible_participants);
      foreach ($users as $id => $user) {
        $participants[] = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
      }
      print implode(',', $participants);
    }
    else {
      print($meeting->field_os2web_meetings_partic['und'][0]['value']);
    }
  }
}

/**
 * Print field location.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_location($meeting_id) {
  if (variable_get('os2dagsorden_hide_meeting_details', FALSE) == FALSE) {
    print (os2dagsorden_meeting_views_common_meeting_location($meeting_id));
  }
}

/**
 * Print field comments.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_comments($meeting_id) {
  if (variable_get('os2dagsorden_hide_meeting_details', FALSE) == FALSE) {
    $meeting = node_load($meeting_id);
    print ($meeting->field_os2web_meetings_comments['und'][0]['value']);
  }
}

/**
 * Print field conditions.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_conditions($meeting_id) {
  if (variable_get('os2dagsorden_hide_meeting_details', FALSE) == FALSE) {
    $meeting = node_load($meeting_id);
    print ($meeting->field_os2web_meetings_conditions['und'][0]['value']);
  }
}

/**
 * Print field referent.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_referent_deltog($meeting_id) {
  $meeting = node_load($meeting_id);
  print($meeting->field_os2web_meetings_referent['und'][0]['value']);

}

/**
 * If the participants cancelled shown be printed.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_meeting_views_meeting_details_fields_participants_afbud_access() {
  return variable_get('os2dagsorden_show_participants_afbud', FALSE);
}

/**
 * Print field participants cancelled.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_participants_afbud($meeting_id) {
  if (variable_get('os2dagsorden_hide_meeting_details', FALSE) == FALSE) {
    $meeting = node_load($meeting_id);
    if (isset($meeting->field_os2web_meetings_particcanl['und'])) {
      print($meeting->field_os2web_meetings_particcanl['und'][0]['value']);
    }
  }
}

/**
 * Print link to all speaker papers.
 *
 * @param int $nid
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_print_all_speaker_papers($nid) {
  if (os2dagsorden_access_helper_meeting_has_speaker_paper($nid)) {
    global $base_url;
    print "<div class='speaker-paper-all-print'>";
    print '<input class="form-submit print-button" type="button" onclick="window.open(';
    print(" ' " . $base_url . '/meeting/' . $nid . '/speaker-papers/print' . " ' ");
    print ')" value="' . variable_get('os2dagsorden_title_meeting_see_all_speakerpapers', 'Se alle talepapirer') . '">';

    print "</div>";
  }
}

/**
 * Wrapper for node_access.
 *
 * @return bool
 *   If user has access to agenda.
 */
function os2dagsorden_meeting_views_meeting_details_dagsorden_access() {
  $q = $_GET['q'];
  $q = explode('/', $q);
  $meeting_id = $q[1];
  $meeting = node_load($meeting_id);
  $committee_tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
  $meeting_type = $meeting->field_os2web_meetings_type['und'][0]['value'];
  if (os2dagsorden_access_helper_is_external_committee($committee_tid) === TRUE) {
    $access_granted = (node_access('view', $meeting) && os2dagsorden_access_helper_check_user_can_see_meeting_in_external_committee($meeting));
  }
  // Forbidding access to premeeting.
  elseif (strpos($meeting_type, OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PREMEETING) === 0) {
    $access_granted = (node_access('view', $meeting) && in_array($committee_tid, os2dagsorden_access_helper_get_user_premeeting_member_of_committees()));
  }
  else {
    $access_granted = node_access('view', $meeting);
  }

  return $access_granted;
}

/**
 * Open agenda title.
 */
function os2dagsorden_meeting_views_meeting_details_fields_dagsorden_header() {
  $meeting_id = arg(1);
  print '<div class="controls-right agenda-level speaker-paper">';
  print os2dagsorden_meeting_views_meeting_details_fields_print_all_speaker_papers($meeting_id);
  print '</div>';
  if (variable_get('os2dagsorden_show_expand_all_button', FALSE)) {
    print '<div class="controls-right agenda-level">';
    print '<a href="" id = "expand_all_agendas" class="form-submit">' . t('Open/Close all') . '</a>';
    print '</div>';
  }
  os2dagsorden_meeting_views_meeting_details_fields_create_bullet_point_button();
  os2dagsorden_meeting_views_meeting_details_fields_create_all_bullet_points_button();

  print '<h3>' . variable_get('os2dagsorden_title_meeting_agenda', 'Dagsorden:') . '</h3>';
}

/**
 * Closed agenda title.
 */
function os2dagsorden_meeting_views_meeting_details_fields_lukket_dagsorden_header() {
  print '<h3>' . variable_get('os2dagsorden_title_meeting_closed_agenda', 'Lukket Dagsorden:') . '</h3>';
}

/**
 * Prints notes indicator.
 *
 * @param int $bp_id
 *   Bullet point nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_indicator_bullet_point_has_sp($bp_id) {
  os2dagsorden_meeting_views_common_bullet_point_has_sp($bp_id);
}

/**
 * Prints notes indicator.
 *
 * @param int $nid
 *   Bullet point nid.
 * @param bool $is_closed
 *   Is bullet ppint closed.
 * @param bool $is_personal
 *   Is bullet ppint personal.
 */
function os2dagsorden_meeting_views_meeting_details_fields_indicator_bullet_point_has_notes($nid, $is_closed, $is_personal) {
  $annotations = os2dagsorden_annotator_get_notes_by_bullet_point_id($nid);
  if (!empty($annotations)) {
    print '<div class="indicator-has-notes"></div>';
  }
  elseif (!os2dagsorden_access_helper_bullet_point_has_speaker_paper($nid, variable_get('os2dagsorden_shared_speaker_paper', TRUE)) && !$is_closed && !$is_personal) {
    print '<div class="indicator-empty"></div>';
  }
}

/**
 * Prints bullet point title.
 *
 * @param string $title
 *   Bullet point title.
 * @param int $meeting_id
 *   Meeting nid.
 * @param int $bp_id
 *   Bullet point nid.
 */
function os2dagsorden_meeting_views_meeting_details_fields_bullet_point_title($title, $meeting_id, $bp_id) {
  global $base_url;

  $bp = node_load($bp_id);
  if (variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE)) {
    $publish_level = intval($bp->field_os2web_meetings_bul_plevel['und'][0]['value']);
    $title = os2dagsorden_access_helper_get_bullet_point_title($meeting_id, $bp_id, $title, $publish_level);
  }
  if (variable_get('os2dagsorden_show_bullet_case_nr', FALSE)) {
    if ($bp->field_os2web_meetings_bul_case['und'] && !empty($bp->field_os2web_meetings_bul_case['und'][0]['value'])) {
      $title .= ' (' . $bp->field_os2web_meetings_bul_case['und'][0]['value'] . ')';
    }
  }

  if (!variable_get('os2dagsorden_show_bp_title_system_text', TRUE)) {
    $title = os2dagsorden_meeting_views_common_fields_strip_bullet_point_number($title);
  }
  else {
    $meeting = node_load($meeting_id);
    $tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
    $committee = taxonomy_term_load($tid);
    if (isset($meeting->field_os2web_meetings_punkt_num['und'])) {
      if (!$meeting->field_os2web_meetings_punkt_num['und'][0]['value']) {
       $title = os2dagsorden_meeting_views_common_fields_strip_bullet_point_number($title);
      }
    }
    elseif (isset($committee->field_os2web_meetings_punkt_num['und']) && !$committee->field_os2web_meetings_punkt_num['und'][0]['value']) {
      $title = os2dagsorden_meeting_views_common_fields_strip_bullet_point_number($title);
    }
  }
  $is_closed = $bp->field_os2web_meetings_bul_closed['und'][0]['value'];
  $is_personal = $bp->field_os2web_meetings_bul_pers['und'][0]['value'];

  if (os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bp_id) || os2dagsorden_access_helper_check_bullet_point_has_public_content($bp_id)) {
    if ($is_closed && variable_get('os2dagsorden_open_closed_bullet_in_new_tab', FALSE)) {
      print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '" target="_blank"  class="first-child poshy_tip"' . $tooltip . '>';
    }
    else {
      print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '" class="first-child poshy_tip"' . $tooltip . '>';
    }
    os2dagsorden_meeting_views_meeting_details_fields_indicator_bullet_point_has_sp($bp_id);

    if ($is_closed) {
      print '<div class="indicator-unlocked"></div>';
    }
    if ($is_personal) {
      print '<div class="indicator-personal"></div>';
    }

    os2dagsorden_meeting_views_meeting_details_fields_indicator_bullet_point_has_notes($bp_id, $is_closed, $is_personal);
    print $title . '</a>';
  }
  else {
    print '<a class="first-child">';
    if ($is_closed) {
      print '<div class="indicator-unlocked"></div>';
    }
    if ($is_personal) {
      print '<div class="indicator-personal"></div>';
    }

    print $title . '</a>';
  }
}

/**
 * Prints attachments with text.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_meeting_details_fields_bullet_point_attachments_with_text($meeting_id, $bp_id) {
  global $user, $base_url;
  $bullet_point = node_load($bp_id);
  if (isset($bullet_point->field_os2web_meetings_attach['und'])) {
    print '<div class="item-list pos-rel">';
    print '<ul class="ul-item-list-dagsordenspunkt">';
    if (variable_get('os2dagsorden_show_bp_comname', FALSE) && isset($bullet_point->field_os2web_meetings_bul_com_n['und'])) {
      print '<div>Sagen afgøres i: ' . $bullet_point->field_os2web_meetings_bul_com_n['und'][0]['value'] . '</div>';
    }
    if ($bullet_point->field_os2web_meetings_bul_closed['und'][0]['value'] == 1 && !os2dagsorden_meeting_views_meeting_details_show_closed_bullet_point_content($meeting_id, $bp_id)) {
      print "<i>Dagsordenpunktets indhold er skjult fordi dagsordenspunktet er lukket.
      Klik på dagsordenspunktets overskrift for at få indeholdet frem.</i>";
    }
    elseif ($bullet_point->field_os2web_meetings_bul_pers['und'][0]['value'] == 1 && !os2dagsorden_meeting_views_meeting_details_show_closed_bullet_point_content($meeting_id, $bp_id)) {
      print "<i>Dagsordenpunktets indhold er skjult fordi dagsordenspunktet er et personpunkt.
      Klik på dagsordenspunktets overskrift for at få indeholdet frem.</i>";
    }
    else {
      $bilags = array();
      $cases = array();
      $all_attachments = array();
      foreach ($bullet_point->field_os2web_meetings_attach['und'] as $attachment_id) {
        $hash = $attachment_id['target_id'];
        $all_attachments[$hash] = $attachment_id;
      }

      foreach ($all_attachments as $attachment_id) {
        $attachment = node_load($attachment_id['target_id']);

        // If is bilag.
        if (!empty($attachment->field_os2web_meetings_bpa_file['und'])) {
          // Saving bilags to the end.
          if ($attachment->field_os2web_meetings_bpa_type['und'][0]['value'] === 'Sager') {
            $cases[] = $attachment;
          }
          else {
            $bilags[] = $attachment;
          }

        }
        // Not bilag.
        else {
          print '<li class="non-bilag">';

          $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
          print '<a href="' . $link . '" class="first-child last-child"><div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';
          if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
            print '<div class="attachment-modify-links-container"><a href="' . $GLOBALS['base_url'] . '/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment/' . $attachment->nid . '" class="edit-meeting-btn" rel="lightframe[|width:800px; height:500px; scrolling: auto;]"><div class="indicator-edit-agenda"></div>&nbsp;</a>'
              . '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment/' . $attachment->nid . '/delete" class="edit-meeting-btn form-submit-delete"><div class="indicator-delete-agenda"></div>'
              . '&nbsp;</a></div>';
          }
          // Attachment text - if any.
          if (!empty($attachment->field_os2web_meetings_bpa_body['und'][0]['value'])) {
            print '<div class="attachment_text_container">';

            print '<div id = "bpa_' . $meeting_id . '_' . $bp_id . '_' . $attachment->nid . '" class="attachment_text_full_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';
            print 'Vent venligst...<div class="throbber"></div>';
            print '</div>';

            print '</div>';
          }
          print '</li>';
        }
      }

      if ($bullet_point->title !== 'Bilag' && $bullet_point->title !== 'Sager') {
        if (!empty($cases) && !(variable_get('os2dagsorden_show_case_bullet_point', FALSE)) && variable_get('os2dagsorden_show_bilag_bullet_point', FALSE)) {
          _os2dagsorden_meeting_views_meeting_details_bilag_cases($cases, $meeting_id, $bp_id);
          if (!empty($bilags)) {
            _os2dagsorden_meeting_views_meeting_details_bilag_cases_bullet_point('Bilag', $bilags, $meeting_id, $bp_id, 'bilags');
          }
        }
        else {
          if (!empty($bilags)) {
            if (variable_get('os2dagsorden_show_bilag_bullet_point', FALSE)) {
              _os2dagsorden_meeting_views_meeting_details_bilag_cases_bullet_point('Bilag', $bilags, $meeting_id, $bp_id, 'bilags');
            }
            else {
              _os2dagsorden_meeting_views_meeting_details_bilag_cases($bilags, $meeting_id, $bp_id);
            }
          }
          if (!empty($cases)) {
            if (variable_get('os2dagsorden_show_case_bullet_point', FALSE)) {
              _os2dagsorden_meeting_views_meeting_details_bilag_cases_bullet_point(variable_get('os2dagsorden_sager_group_title', 'Sager'), $cases, $meeting_id, $bp_id, 'cases');
            }
            else {
              _os2dagsorden_meeting_views_meeting_details_bilag_cases($cases, $meeting_id, $bp_id);
            }

          }
        }
      }
      else {
        _os2dasorden_meeting_views_meeting_details_bilag_cases_with_content($bilags, $meeting_id, $bp_id);
        _os2dasorden_meeting_views_meeting_details_bilag_cases_with_content($cases, $meeting_id, $bp_id);
      }

      print '</div>';
    }
    print '</ul>';
    print '</div>';
  }
}

/**
 * Prints all cases.
 *
 * @param mixed $elements
 *   Attachments.
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function _os2dagsorden_meeting_views_meeting_details_bilag_cases($elements, $meeting_id, $bp_id) {
  global $base_url;
  foreach ($elements as $attachment) {
    print '<li class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    }
    else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';
      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }
      else {
        print '<div class="indicator-empty"></div>';
      }
      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>';
      print $attachment->title . '</a>';
      if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
        print '<div class="attachment-modify-links-container"><a href="' . $GLOBALS['base_url'] . '/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment/' . $attachment->nid . '" class="edit-meeting-btn" rel="lightframe[|width:800px; height:500px; scrolling: auto;]"><div class="indicator-edit-agenda"></div>&nbsp;</a>'
              . '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment/' . $attachment->nid . '/delete" class="edit-meeting-btn form-submit-delete"><div class="indicator-delete-agenda"></div>'
              . '&nbsp;</a></div>';
      }
      // Attachment text - if any.
      if (variable_get('os2dagsorden_show_bilag', TRUE)) {
        if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
          print '<div class="attachment_text_container">';

          print '<div id = "bpa_' . $meeting_id . '_' . $bp_id . '_' . $attachment->nid . '" class="attachment_text_trimmed_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';

          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';
          print('<a class="first-child last-child" href="' . $link . '" target="_blank">Læs mere...</a>');

          print '</div>';
          if (variable_get('os2dagsorden_show_bilag_pages_amount', FALSE)) {
            if ((int) $attachment->field_os2web_meetings_bpa_pages['und'][0]['value'] > 1) {
              print '<div class = "bilag-pages">' . t(' @pages sider', array('@pages' => $attachment->field_os2web_meetings_bpa_pages['und'][0]['value'])) . '</div>';
            }
            else {
              print '<div class = "bilag-pages">' . t(' @pages side', array('@pages' => $attachment->field_os2web_meetings_bpa_pages['und'][0]['value'])) . '</div>';
            }
          }
        }
      }
    }
    else {
      print '<a class="first-child last-child">';

      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }
      else {
        print '<div class="indicator-empty"></div>';
      }

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>';
      print $attachment->title . '</a>';
    }
    print '</li>';
  }
}

/**
 * Prints single bullet point.
 *
 * @param string $bp_title
 *   Title of bullet point.
 * @param mixed $elements
 *   Attachments.
 * @param int $meeting_id
 *   Meeting nid.
 * @param int $bp_id
 *   Bullet point id.
 * @param string $attachment_type
 *   Type of the attachment.
 */
function _os2dagsorden_meeting_views_meeting_details_bilag_cases_bullet_point($bp_title, $elements, $meeting_id, $bp_id, $attachment_type) {
  global $base_url;
  print '<li class="bilags_cases ' . $attachment_type . '">';
  print '<div class="indicator-has-no-notes"></div>';
  print '<a class="first-child last-child">' . $bp_title . '</a>';
  print '<div class="bilags_cases_container">';

  // Going through all bilags.
  foreach ($elements as $attachment) {
    print '<div class="bilag">';

    if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
      print '<div class="indicator-unlocked"></div>';
    }
    else {
      print '<div class="indicator-empty"></div>';
    }

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    }
    else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>';
    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">' . $attachment->title . '</a>';
    }
    else {
      print '<a class="first-child last-child">' . $attachment->title . '</a>';
    }
    if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
      print '<div class="attachment-modify-links-container"><a href="' . $GLOBALS['base_url'] . '/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment/' . $attachment->nid . '" class="edit-meeting-btn" rel="lightframe[|width:800px; height:500px; scrolling: auto;]"><div class="indicator-edit-agenda"></div>&nbsp;</a>'
              . '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment/' . $attachment->nid . '/delete" class="edit-meeting-btn form-submit-delete"><div class="indicator-delete-agenda"></div>'
              . '&nbsp;</a></div>';
    }
    print '</div>';
  }

  print '</div>';
  print '</li>';
}

/**
 * Prints bilag with content.
 *
 * @param mixed $elements
 *   Array of attachments.
 * @param int $meeting_id
 *   Meeting nid.
 * @param int $bp_id
 *   Bullet point nid.
 */
function _os2dasorden_meeting_views_meeting_details_bilag_cases_with_content($elements, $meeting_id, $bp_id) {
  global $base_url;

  // Going through all bilags.
  foreach ($elements as $attachment) {
    print '<li class="bilag">';

    if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
      print '<div class="indicator-unlocked"></div>';
    }
    else {
      print '<div class="indicator-empty"></div>';
    }

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    }
    else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>';
    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">' . $attachment->title . '</a>';

      // Attachment text - if any.
      if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
        // Should bilag be visible.
        if (variable_get('os2dagsorden_show_bilag', TRUE)) {
          print '<div class="attachment_text_container">';

          print '<div id = "bpa_' . $meeting_id . '_' . $bp_id . '_' . $attachment->nid . '" class="attachment_text_trimmed_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';

          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';
          print('<a class="first-child last-child" href="' . $link . '" target="_blank">Læs mere...</a>');

          print '</div>';
        }
      }
    }
    else {
      print '<a class="first-child last-child">' . $attachment->title . '</a>';
    }
    print '</li>';
  }
}

/**
 * Prints to-text class is the text is hidden for speaker paper link.
 */
function os2dagsorden_meeting_views_meeting_details_fields_speaker_paper_link_class() {
  if (variable_get('os2dagsorden_title_meeting_new_speaker_paper_hide', FALSE)) {
    print 'no-text';
  }
}

/**
 * Prints speaker paper link.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 * @param string $bp_title
 *   Bullet point title.
 */
function os2dagsorden_meeting_views_meeting_details_fields_speaker_paper_link($meeting_id, $bp_id, $bp_title) {
  global $base_url;
  if (!os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bp_id)) {
    print "<a>&nbsp;</a>";
  }
  else {
    if (os2dagsorden_access_helper_bullet_point_has_speaker_paper($bp_id, variable_get('os2dagsorden_shared_speaker_paper', TRUE))) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id;
      if (!variable_get('os2dagsorden_title_meeting_new_speaker_paper_hide', FALSE)) {
        print '<a href="' . $link . '"><div class="indicator-has-speaker-paper"></div>Talepapir</a>';
      }
      else {
        print '<a href="' . $link . '"><div class="indicator-has-speaker-paper"></div>&nbsp;</a>';
      }
    }
    else {
      $q = $_GET['q'];
      $link = $base_url . '/node/add/os2web-meetings-spaper?edit[field_os2web_meetings_sp_bullet][und][0][target_id]=' . $bp_title . ' (' . $bp_id . ')' . '&destination=' . $q;

      $text = variable_get('os2dagsorden_title_meeting_new_speaker_paper', 'Nyt talepapir');
      if (!variable_get('os2dagsorden_title_meeting_new_speaker_paper_hide', FALSE)) {
        print '<a href="' . $link . '" title="' . $text . '"><div class="indicator-create-speaker-paper"></div>' . $text . '</a>';
      }
      else {
        print '<a href="' . $link . '" title="' . $text . '"><div class="indicator-create-speaker-paper"></div>&nbsp;</a>';
      }
    }
  }
}

/**
 * Prints bullet point print link.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_meeting_details_fields_bullet_point_print_link($meeting_id, $bp_id) {
  global $base_url;
  if (variable_get('os2dagsorden_show_print_icon', FALSE)) {
    if (os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bp_id)) {
      print '<a href="' . $base_url . '/print/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/print"><span class="bullet-point-print-link"></span>&nbsp;</a>';
    }
    else {
      print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '">&nbsp;</a>';
    }
  }
}

/**
 * Prints bullet point sent to friend link.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_meeting_details_fields_bullet_point_send_link($meeting_id, $bp_id) {
  global $base_url;
  if (module_exists('os2web_print_send_to_friend') && variable_get('os2dagsorden_show_send_to_friend_icon', FALSE)) {
    if (os2dagsorden_access_helper_check_user_can_send_bullet_point($meeting_id, $bp_id)) {
      print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/send_to_friend_form" rel="lightframe"><span class="bullet-point-send-link"></span>&nbsp;</a>';
    }
    else {
      print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '">&nbsp;</a>';
    }
  }
}

/**
 * Prints bullet point indentifier link.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_meeting_details_fields_bullet_point_case_identifier_link($meeting_id, $bp_id) {
  if (variable_get('os2dagsorden_show_bullet_case_id_link', FALSE) && variable_get('os2dagsorden_bullet_case_id_link_url', FALSE)) {
    $bullet_point = node_load($bp_id);

    $case_link_url = variable_get('os2dagsorden_bullet_case_id_link_url');
    if (!empty($bullet_point->field_os2web_meetings_bul_caseid['und'][0]['value'])) {
      $caseId = $bullet_point->field_os2web_meetings_bul_caseid['und'][0]['value'];
      print '<a href="' . $case_link_url . $caseId . '" target="_blank"><div class="popup-link"></div>' . t('Sag') . '</a>';
    }
    else {
      print '<a>&nbsp;</a>';
    }
  }
}

/**
 * Determines is the bullet point can be filtered out.
 *
 * @param int $nid
 *   Bullet point nid.
 * @param bool $filter_opened
 *   Flag to filter the opened opened bp.
 *
 * @return bool
 *   True if skip, false if show.
 */
function os2dagsorden_meeting_views_meeting_details_fields_filter_bullet_point($nid, $filter_opened) {
  $bullet_point = node_load($nid);
  if (intval($bullet_point->field_os2web_meetings_bul_gener['und'][0]['value']) == 1) {
    return TRUE;
  }
  // Put inline.
  if (variable_get('os2dagsorden_group_closed_bullet_points', 'false') === 'false') {
    // Accept every type.
    return !$filter_opened;
  }
  // Put separately.
  else {
    // Accept opened.
    if ($filter_opened) {
      if ($bullet_point->field_os2web_meetings_bul_closed['und'][0]['value'] == 0) {
        return FALSE;
      }
      else {
        return TRUE;
      }
    }
    // Accept closed.
    else {
      if ($bullet_point->field_os2web_meetings_bul_closed['und'][0]['value'] == 1) {
        return FALSE;
      }
      else {
        return TRUE;
      }
    }
  }
}

/**
 * Returns if bullet point content is to be shown.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 *
 * @return bool
 *   True is yes, false is no.
 */
function os2dagsorden_meeting_views_meeting_details_show_closed_bullet_point_content($meeting_id, $bp_id) {
  if (variable_get('os2dagsorden_show_closed_bullet_point_content', FALSE) && (os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bp_id) || os2dagsorden_access_helper_check_bullet_point_has_public_content($bp_id))) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Retuns if user has access to closed dagsorden.
 *
 * @return bool
 *   True is yes, false if no.
 */
function os2dagsorden_meeting_views_meeting_details_lukket_dagsorden_access() {
  if (os2dagsorden_meeting_views_meeting_details_dagsorden_access()) {
    return variable_get('os2dagsorden_group_closed_bullet_points', 'false') === 'true';
  }
}

/**
 * Prints tillags dagsorden header.
 */
function os2dagsorden_meeting_views_meeting_details_fields_tilleags_dagsorden_header() {
  print '<h3>' . 'Tillægs ' . variable_get('os2dagsorden_title_meeting_agenda', 'Dagsorden:') . '</h3>';
}

/**
 * Prints create bullet point button.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_meeting_details_fields_dagsorden_create_bullet_point($meeting_id) {
  if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
    print '<div class="controls-right agenda-level">';
    print '<a href="" class="form-submit">' . t('Open/Close all') . '</a>';
    print '</div>';
  }
}

/**
 * Open create BP button.
 */
function os2dagsorden_meeting_views_meeting_details_fields_create_bullet_point_button() {
  $meeting_id = arg(1);
  if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
    print '<div class="controls-right agenda-level create-agenda">';
    print '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point" class="form-submit" rel="lightframe[|width:800px; height:500px; scrolling: auto;]">' . t('Operet punkt') . '</a>';
    print '</div>';
  }
}

/**
 * Open create BP button.
 */
function os2dagsorden_meeting_views_meeting_details_fields_create_all_bullet_points_button() {
  $meeting_id = arg(1);
  if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
    print '<div class="controls-right agenda-level create-agenda">';
    print '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point/create_from_file" class="form-submit" rel="lightframe[|width:800px; height:500px; scrolling: auto;]">' . t('Opret fra filer') . '</a>';
    print '</div>';
  }
}

