<?php

/**
 * @file
 * Os2dagsorden_create_agenda.
 *
 * PHP version 5
 *
 * @category OS2Dagsorden
 * @package Os2dagsorden_Create_Agenda
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/licenses/gpl-2.0.html GNU General Public License, version 2
 * @link http://bellcom.dk
 */

 /**
  * Generates the form for meeting creating/modifying.
  *
  * @param mixed $form
  *   The form to be created.
  * @param mixed $form_state
  *   The current state of the form.
  *
  * @return mixed
  *   The created form.
  */
function os2dagsorden_create_agenda_meeting($form, &$form_state, $meeting_id, $bp_id = NULL, $bpa_id = NULL) {
  if (!empty($form_state['page_num']) && $form_state['page_num'] === 2) {
    return os2dagsorden_create_agenda_bullet_point($form, $form_state, $bullet_point_id);
  }
  elseif (!empty($form_state['page_num']) && $form_state['page_num'] === 3) {
    return os2dagsorden_create_agenda_bpa($form, $form_state);
  }
  elseif (!empty($form_state['page_num']) && $form_state['page_num'] === 4) {
    return os2dagsorden_create_agenda_meeting_create_user_form($form, $form_state);
  }
  elseif (isset($bpa_id)) {
    $form_state['meeting_id'] = $meeting_id;
    $form_state['bullet_point_id'] = $bp_id;
    $form_state['bpa_id'] = $bpa_id;
    return os2dagsorden_create_agenda_bpa($form, $form_state, $bp_id, $meeting_id);
  }
  elseif (arg(5) == 'attachment') {
    $form_state['meeting_id'] = $meeting_id;
    $form_state['bullet_point_id'] = $bp_id;
    return os2dagsorden_create_agenda_bpa($form, $form_state, $bp_id, $meeting_id);
  }
  elseif (isset($bp_id) || arg(3) == 'bullet-point') {
    $form_state['meeting_id'] = $meeting_id;
    $form_state['bullet_point_id'] = $bp_id;
    return os2dagsorden_create_agenda_bullet_point($form, $form_state);
  }

  $form_state['page_num'] = 1;

  $form[] = array(
    '#markup' => '<h1 class="title">' . t('Meeting') . '</h1>',
  );

  // Adding general fields.
  $form = os2dagsorden_create_agenda_meeting_form_add_general_fields($form, $form_state);

  // Pass the maximum file size in bytes - 2 MB.
  $max_filesize_bytes = variable_get('os2dagsorden_plupupload_max_filesize', 2) * 1024 * 1024;
  $allowed_extensions = variable_get('os2dagsorden_plupupload_allowed_filetypes', 'doc docx pdf');
  if (variable_get('os2dagsorden_create_agenda_full_agenda_upload_fields_edit_form', FALSE)) {
    // Adding other fields.
    $form['description_opened'] = array(
      '#title' => t('Description opened:'),
      '#type' => 'plupload',
      '#name' => 'files[]',
      '#autoupload' => TRUE,
      '#upload_validators' => array(
        'file_validate_extensions' => array($allowed_extensions),
        'file_validate_size' => array($max_filesize_bytes),
      ),
      '#plupload_settings' => array(
        'multi_selection' => false,
      ),
    );

    $form['description_closed'] = array(
      '#title' => t('Description closed:'),
      '#type' => 'plupload',
      '#name' => 'files[]',
      '#autoupload' => TRUE,
      '#upload_validators' => array(
        'file_validate_extensions' => array($allowed_extensions),
        'file_validate_size' => array($max_filesize_bytes),
      ),
      '#plupload_settings' => array(
        'multi_selection' => false,
      ),
    );
  }

  $form['agenda'] = array(
    '#type' => 'fieldset',
    '#title' => t('Agenda'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="js-agenda-fs" data-meeting_id = ' . $meeting_id . '>',
    '#suffix' => '</div>'
  );

  // Filling the fields if meeting_id is not empty.
  if (!$meeting_id) {
    // If meeting id is empty, try to fill it with form_state meeting_id value.
    $meeting_id = $form_state['meeting_id'];
  }
  $form['create_user'] = array(
    '#type' => 'submit',
    '#prefix' => '<div class="create-user-btn">',
    '#attributes' => array(
      'class' => array('form-add-submit'),
    ),
    '#value' => t('Create user'),
    '#submit' => array('os2dagsorden_create_agenda_meeting_create_user'),
    '#suffix' => '</div>',
  );
  _os2dagsorden_create_agenda_render_drag_and_drop_controls($form, $form_state, $meeting_id);
  if ($meeting_id) {
    // Saving for usage in the submit function.
    $form_state['meeting_id'] = $meeting_id;

    $node = node_load($meeting_id);

    // Filling general fields.
    $form = os2dagsorden_create_agenda_meeting_form_fill_general_fields($form, $node);

    // Filling other fields.
    if (field_get_items('node', $node, 'field_os2web_meetings_full_doc')) {
      $description_opened_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_full_doc'));
      $form['description_opened']['#default_value'] = $description_opened_field['fid'];
    }

    if (field_get_items('node', $node, 'field_os2web_meetings_full_doc_c')) {
      $description_closed_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_full_doc_c'));
      $form['description_closed']['#default_value'] = $description_closed_field['fid'];
    }

    if ($bullet_points_target_ids = field_get_items('node', $node, 'field_os2web_meetings_bullets')) {

      $bullet_points = array();
      foreach ($bullet_points_target_ids as $bp_target) {
        $bp = node_load($bp_target['target_id']);

        // Ordering BPs by number.
        $i = 0;
        while (isset($bullet_points[$bp->field_os2web_meetings_bul_number['und'][0]['value'] . $i])) {
          $i++;
        }

        $bullet_points[$bp->field_os2web_meetings_bul_number['und'][0]['value'] . $i] = $bp;
      }
      ksort($bullet_points);
      foreach ($bullet_points as $bp) {
        $form['agenda'][] = array(
          '#markup' => '<div class="create-agenda-list-item" id="bp_' . $bp->nid . '">',
        );
        $form['agenda'][] = array(
          '#markup' => '<span class="create-agenda-edit-delete-container">',
        );
        $form['agenda'][] = array(
          '#type' => 'image_button',
          '#button_type' => 'submit-edit',
          '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_edit.png',
          '#name' => 'edit_' . $bp->nid,
          '#submit' => array('os2dagsorden_create_agenda_meeting_submit_add_edit_bullet_point'),
          '#bp_nid' => $bp->nid,
        );
        $form['agenda'][] = array(
          '#type' => 'image_button',
          '#button_type' => 'submit-delete',
          '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_delete.png',
          '#name' => 'delete_' . $bp->nid,
          '#submit' => array('os2dagsorden_create_agenda_meeting_submit_delete_bullet_point'),
          '#bp_nid' => $bp->nid,
          '#ajax' => array(
            'callback' => 'os2dagsorden_create_agenda_bullet_point_delete_ajax',
            'wrapper' => 'js-agenda-fs',
            'method' => 'replace',
            'effect' => 'fade',
          ),
        );

        $form['agenda'][] = array(
          // '<span class="create-agenda-edit-delete-container">',.
          '#markup' => '</span>',
        );

        $form['agenda'][] = array(
          '#markup' => '<span class="create-agenda-privacy-container">',
        );
        $closed_field = array_pop(field_get_items('node', $bp, 'field_os2web_meetings_bul_closed'));
        if ($closed_field['value']) {
          $form['agenda'][] = array(
            '#markup' => '<span class="indicator-form-unlocked"></span>',
          );
        }
        else {
          $form['agenda'][] = array(
            '#markup' => '<span class="indicator-form-empty"></span>',
          );
        }

        $personal_field = array_pop(field_get_items('node', $bp, 'field_os2web_meetings_bul_pers'));
        if ($personal_field['value']) {
          $form['agenda'][] = array(
            '#markup' => '<span class="indicator-form-personal"></span>',
          );
        }
        else {
          $form['agenda'][] = array(
            '#markup' => '<span class="indicator-form-empty"></span>',
          );
        }

        $form['agenda'][] = array(
          // '<span class="create-agenda-privacy-container">',.
          '#markup' => '</span>',
        );

        $form['agenda'][] = array(
          '#markup' => '[' . $bp->field_os2web_meetings_bul_number['und'][0]['value'] . '] ' . $bp->title,
        );
        $form['agenda'][] = array(
          // '<div class="create-agenda-list-item">',.
          '#markup' => '</div>',
        );
      }
    }
  }
  else if ($meeting_id = os2dagsorden_create_agenda_get_latest_meeting_created_by_user()) {
    $node = node_load($meeting_id);

    // Filling general fields.
    $form = os2dagsorden_create_agenda_meeting_form_fill_general_fields($form, $node);

  }


  if (!isset($form['agenda'][0])) {
    $form['agenda']['#access'] = FALSE;
  }

  // Finishing form.
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/css/form_theme.css',
  );
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/js/form_js.js',
  );

  $form['save_meeting'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-save-submit'),
    ),
    '#value' => t('Save'),
    '#submit' => array('os2dagsorden_create_agenda_meeting_submit'),
    '#ajax' => array(
      'callback' => 'os2dagsorden_create_agenda_meeting_create_ajax',
      'wrapper' => 'os2dagsorden-create-agenda-meeting',
      'method' => 'replace',
      'effect' => 'fade',
    )
  );
  /*$form[] = array(
    '#markup' => '&nbsp;',
  );
  $form['save_meeting_and_add_bullet_point'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-add-submit'),
    ),
    '#value' => t('Save & Add bullet point'),
    '#submit' => array('os2dagsorden_create_agenda_meeting_submit_add_edit_bullet_point'),
  );*/
  $form[] = array(
    '#markup' => '</div>',
  );

  return $form;
}

/**
 * Callback for deleting bullet point.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_delete_ajax($form, $form_state) {
  return $form['agenda'];
}

/**
 * Saves the meeting - either by creating new or modifying the existing.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return object
 *   the meeting node.
 */
function os2dagsorden_create_agenda_save_meeting($form, $form_state) {
  $node = os2dagsorden_create_agenda_populate_meeting_general($form, $form_state);

  $input = $form_state['input'];
  if (variable_get('os2dagsorden_create_agenda_full_agenda_upload_fields_edit_form', FALSE)) {
    // File extraction.
    $description_opened = $form_state['values']['description_opened'];
    $description_closed = $form_state['values']['description_closed'];

    if (!empty($description_opened)) {
     foreach ($description_opened as $description_file) {
      if ($description_file['status'] == 'done') {
        // Resaving file permanently.
        $source = $description_file['tmppath'];
        $destination = file_stream_wrapper_uri_normalize(variable_get('os2dagsorden_plupupload_filedir_uri', 'private://manual_agendas') . '/' . $description_file['name']);
        $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);
        $description_file = plupload_file_uri_to_object($destination);
        file_save($description_file);
        $description_file->status = FILE_STATUS_PERMANENT;
        $description_file->display = 1;
        file_save($description_file);
        $node->field_os2web_meetings_full_doc['und'][0] = (array) $description_file;
        break;
       }
      }
    }
    else {
      if (field_get_items('node', $node, 'field_os2web_meetings_full_doc')) {
        $description_opened_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_full_doc'));
        $to_del_file = file_load($description_opened_field['fid']);
        unset($node->field_os2web_meetings_full_doc['und']);
        file_delete($to_del_file);
      }
    }

    if (!empty($description_closed)) {
      foreach ($description_closed as $description_file) {
        if ($description_file['status'] == 'done') {
          // Resaving file permanently.
          $source = $description_file['tmppath'];
          $destination = file_stream_wrapper_uri_normalize(variable_get('os2dagsorden_plupupload_filedir_uri', 'private://manual_agendas') . '/' . $description_file['name']);
          $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);
          $description_file = plupload_file_uri_to_object($destination);
          file_save($description_file);
          $description_file->status = FILE_STATUS_PERMANENT;
          $description_file->display = 1;
          file_save($description_file);
          $node->field_os2web_meetings_full_doc_c['und'][0] = (array) $description_file;
          break;
        }
      }
    }
    else {
      if (field_get_items('node', $node, 'field_os2web_meetings_full_doc_c')) {
        $description_closed_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_full_doc_c'));
        $to_del_file = file_load($description_closed_field['fid']);
        unset($node->field_os2web_meetings_full_doc_c['und']);
        file_delete($to_del_file);
      }
    }
  }
  $node->field_os2web_meetings_type['und'][0]['value'] = $input['status'];
  node_submit($node);
  if (variable_get('os2dagsorden_import_full_agenda_as_attachments', FALSE)) {
    if ($general_item_id = os2dagsorden_create_agenda_create_general_item($node)) {
      $node->field_os2web_meetings_bullets[LANGUAGE_NONE][]['target_id'] = $general_item_id;
    }
  }
  node_save($node);
  return $node;
}

/**
 * Helper functions, meeting form submit.
 *
 * Calls meeting save and redirects to the calendar view.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_meeting_submit($form, &$form_state) {
  $node = os2dagsorden_create_agenda_save_meeting($form, $form_state);

  $form_state['redirect'] = 'meeting/' . $node->nid;
}

/**
 * Calls os2dagsorden_create_agenda_save_meeting.
 *
 * And redirects to bullet point creation/modification page.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_meeting_submit_add_edit_bullet_point($form, &$form_state) {
  $node = os2dagsorden_create_agenda_save_meeting($form, $form_state);

  $form_state['meeting_id'] = $node->nid;
  // If empty - will be treated as new node creation.
  $form_state['bullet_point_id'] = $form_state['triggering_element']['#bp_nid'];

  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper functions to delete bullet point.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_meeting_submit_delete_bullet_point($form, &$form_state) {
  $bp_id = $form_state['triggering_element']['#bp_nid'];
  os2dagsorden_create_agenda_remove_bullet_point_from_meeting($form_state['meeting_id'], $bp_id);

  node_delete($bp_id);

  $form_state['page_num'] = 1;
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper functions to delete bullet point via ajax.
 *
 * @param mixed $meeting_id
 *   The meeting nid.
 * @param mixed $bullet_point_id
 *   The bullet point nid.
 */
function os2dagsorden_create_agenda_delete_bullet_point_ajax_call($meeting_id, $bullet_point_id) {
  os2dagsorden_create_agenda_remove_bullet_point_from_meeting($meeting_id, $bullet_point_id);
  node_delete($bullet_point_id);
  drupal_json_output(array('status' => 'OK', 'element' => 'bp'));
}

/**
 * Removes the bullet point from the meeting.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 */
function os2dagsorden_create_agenda_remove_bullet_point_from_meeting($meeting_id, $bullet_point_id) {
  $meeting = node_load($meeting_id);
  if (field_get_items('node', $meeting, 'field_os2web_meetings_bullets')) {
    $bullet_points = field_get_items('node', $meeting, 'field_os2web_meetings_bullets');
    foreach ($bullet_points as $key => $bp) {
      if ($bp['target_id'] === $bullet_point_id) {
        unset($meeting->field_os2web_meetings_bullets['und'][$key]);
      }
    }
  }

  $meeting->field_os2web_meetings_bullets['und'] = array_values($meeting->field_os2web_meetings_bullets['und']);
  node_save($meeting);
}

/**
 * Callback for creating meeting.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_meeting_create_ajax($form, &$form_state) {
  global $base_url;
  if (!empty(form_get_errors())) {
    return $form;
  }
  else {
    $commands = array();
    $commands[] = array(
      'command' => 'closeLightboxAndRedirect',
      'redirect' => $base_url . '/' . $form_state['redirect']
    );
    return array('#type' => 'ajax', '#commands' => $commands);
  }
}

/**
 * Helper functions to sort bullet points via ajax.
 */
function os2dagsorden_create_agenda_sort_bp_ajax_call() {
  parse_str($_POST['attachments'], $attachments);
  $meeting = node_load($_POST['meeting_id']);
  unset($meeting->field_os2web_meetings_bullets['und']);
  $attachments = array_pop($attachments);
  $bullet_points = array_values(node_load_multiple($attachments));
  foreach ($bullet_points as $key => $bp) {
    if ((int)$bp->field_os2web_meetings_bul_number['und'][0]['value'] != (int) $key + 1)  {
      if (preg_match_all('/Punkt ([0-9]*)/i', $bp->title, $matches) === 1) {
        $bp->title = str_replace($matches[0], 'Punkt ' . (string) ($key + 1), $bp->title);
      }
      $bp->field_os2web_meetings_bul_number['und'][0]['value'] = (int) $key + 1;
      node_save($bp);
    }
    $meeting->field_os2web_meetings_bullets['und'][]['target_id'] = $nid;
  }
  node_save($meeting);
}

/**
 * Helper functions to find latest created meeting.
 *
 * * @return mixed
 *   the meeting nid if found, FALSE otherwise.
 */
function os2dagsorden_create_agenda_get_latest_meeting_created_by_user() {
  global $user;
  $timastamp = 0;
  if (variable_get('os2dagsorden_create_agenda_period_to_store_latest_created_meeting_data', -1) > 0) {
    $timastamp = time() - variable_get('os2dagsorden_create_agenda_period_to_store_latest_created_meeting_data', -1) * 3600;
  }
  $meeting = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('n.type', 'os2web_meetings_meeting')
    ->condition('n.uid', $user->uid)
    ->condition('created', $timastamp, '>')
    ->orderBy('created', 'DESC')
    ->range(0, 1)
    ->execute()
    ->fetchAll();
  if ($meeting) {
    $meeting = array_pop($meeting);
    return $meeting->nid;
  }
  return FALSE;
}

function os2dagsorden_create_agenda_create_general_item($node) {
  $items = array (
    'full_doc' => variable_get('os2dagsorden_general_bullet_full_doc_title', t('Åben samlet dokument')),
    'full_doc_c' => variable_get('os2dagsorden_general_bullet_full_doc_title', t('Åben samlet dokument')) . '(Lukket)',
  );
  $doc_file_exists = FALSE;
  foreach ($items as $key => $title) {
    $field = 'field_os2web_meetings_' . $key;
    if (isset($node->$field[LANGUAGE_NONE][0])) {
      $doc_file_exists = TRUE;
      break;
    }
  }
  if ($doc_file_exists) {
    $bullet_point = _os2dagsorden_access_helper_get_genaral_bullet_point($node->nid);

  if (!$bullet_point) {
    $bullet_point = new stdClass();
    $bullet_point->type = "os2web_meetings_bullet";
    $bullet_point->language = LANGUAGE_NONE;
    $bullet_point->uid = 1;
    $bullet_point->title =variable_get('os2dagsorden_general_bullet_point_title', t('Generalt'));
    // Set some default values.
    node_object_prepare($bullet_point);
    $bullet_point->field_os2web_meetings_bul_gener[LANGUAGE_NONE][0]['value'] = 1;
    $bullet_point->field_os2web_meetings_bul_number[LANGUAGE_NONE][0]['value'] = 0;
    node_submit($bullet_point);
    node_save($bullet_point);
  }
  $existing_attachments = array();
  if ($bullet_point->field_os2web_meetings_attach['und']) {
    foreach($bullet_point->field_os2web_meetings_attach['und'] as $attachnemt_nid) {
      $attachment =  node_load($attachnemt_nid['target_id']);
      foreach ($items as $key => $title) {
        if ($attachment->title === $title) {
          $existing_attachments[$key] = $attachment;
          break;
        }
      }
    }
  }
  $attachment_nids = array();
  foreach ($items as $key => $title) {
    $field = 'field_os2web_meetings_' . $key;
    $schedule_to_convert = FALSE;
    if (isset($node->$field[LANGUAGE_NONE][0])) {
      $file = file_load($node->$field[LANGUAGE_NONE][0]['fid']);
      $file->display = 1;
      if (isset($existing_attachments[$key])) {
        $attachment_node = $existing_attachments[$key];
      }
      else {
        $attachment_node = new stdClass();
        $attachment_node->type = "os2web_meetings_bullet_attach";
        $attachment_node->language = LANGUAGE_NONE;
        $attachment_node->uid = 1;
        $attachment_node->title = $title;
        node_object_prepare($attachment_node);
        node_submit($attachment_node);
        node_save($attachment_node);
      }
      if (isset($attachment_node->field_os2web_meetings_bpa_file['und'][0])) {
        if ((int) $enclosure_node->field_os2web_meetings_bpa_file['und'][0]['filesize'] !== $file->filesize) {
          $attachment_node->field_os2web_meetings_bpa_file['und'][0] = (array) $file;
          $schedule_to_convert = TRUE;
          node_save($attachment_node);
        }
      }
      else {
        $attachment_node->field_os2web_meetings_bpa_file['und'][0] = (array) $file;
        $schedule_to_convert = TRUE;
        node_save($attachment_node);
      }

      if($schedule_to_convert) {
        // PDF conversion.
        if (module_exists("os2dagsorden_pdf2htmlex")) {
          // If is PDF, schedule for HTML conversion.
          if (file_get_mimetype($file->uri) === 'application/pdf') {
            os2dagsorden_pdf2htmlex_schedule($file->fid, $attachment_node->nid);
          }
        }
        // PNG  conversion.
        if (module_exists("os2dagsorden_pdf2png_conversion")) {
          // If is PDF, schedule for HTML conversion.
          if (file_get_mimetype($file->uri) === 'application/pdf') {
            os2dagsorden_pdf2png_conversion_schedule($file->fid);
          }
        }
      }
      $attachment_nids[] = array('target_id' => $attachment_node->nid);
    }
  }
  $bullet_point->field_os2web_meetings_attach['und'] =  $attachment_nids;
  node_save($bullet_point);
  return $bullet_point->nid;

  }
  return FALSE;
}

function os2dagsorden_create_agenda_meeting_create_user($form, &$form_state) {
  $form_state['page_num'] = 4;
  $form_state['rebuild'] = TRUE;
}