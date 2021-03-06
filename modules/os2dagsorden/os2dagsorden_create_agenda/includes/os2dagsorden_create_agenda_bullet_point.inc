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
 * Generates the form for bullet point creating/modifying.
 *
 * @param mixed $form
 *   The form to be created.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return mixed
 *   The created form.
 */
function os2dagsorden_create_agenda_bullet_point($form, &$form_state) {
  $form[] = array(
    '#markup' => '<h1 class="title">' . t('Bullet point') . '</h1>',
  );

  $form[] = array(
    '#markup' => '<div class="node">',
  );
  $form['bp_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('Full title of the bullet point'),
  );

  $form['number'] = array(
    '#type' => 'textfield',
    '#title' => t('Number:'),
    '#attributes' => array(
      ' type' => 'number',
    ),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#element_validate' => array('element_validate_number'),
    '#description' => t('The ordinal number of the bullet point'),
    '#default_value' => os2dagsorden_create_agenda_bullet_point_find_last_number($form_state['meeting_id'])
  );

  $form['closed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Closed'),
    '#field_suffix' => '<div class="indicator-form-unlocked"></div>',
  );

  $form['personal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Personal'),
    '#field_suffix' => '<div class="indicator-form-personal"></div>',
  );
  $twenty_mb_bytes = variable_get('os2dagsorden_plupupload_max_filesize', 2) * 1024 * 1024;
  $allowed_extensions = variable_get('os2dagsorden_plupupload_allowed_filetypes', 'doc docx pdf');

  $form['bilags'] = array(
    '#type' => 'plupload',
    '#name' => 'files[]',
    '#title' => t('Upload bilags files'),
    '#autoupload' => variable_get('os2dagsorden_plupupload_autoupload', TRUE),
    '#upload_validators' => array(
      'file_validate_extensions' => array($allowed_extensions),
      'file_validate_size' => array($twenty_mb_bytes),
    ),
   '#prefix' => '<div class="auto">',
   '#suffix' => '</div>',
  );
  if (variable_get('os2dagsorden_plupupload_autoupload', TRUE)) {
    $form['bilags']['#prefix'] = '<div class="field-autoupload">';
    $form['bilags']['#suffix'] = '</div>';
  }
  $form['bpas'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bullet point attachments'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="js-bpas-fs" data-bp_id = ' . $form_state['bullet_point_id'] . '>',
    '#suffix' => '</div>'
  );

  // Filling the fields if meeting_id is not empty.
  if ($form_state['bullet_point_id']) {
    $node = node_load($form_state['bullet_point_id']);
    $form['bp_title']['#default_value'] = $node->title;

    $number = array_pop(field_get_items('node', $node, 'field_os2web_meetings_bul_number'));
    $form['number']['#default_value'] = $number['value'];

    $closed_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_bul_closed'));
    $form['closed']['#default_value'] = $closed_field['value'];

    $personal_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_bul_pers'));
    $form['personal']['#default_value'] = $personal_field['value'];

    if (field_get_items('node', $node, 'field_os2web_meetings_attach')) {
      $bpas = field_get_items('node', $node, 'field_os2web_meetings_attach');
      $bilags = array();
      $non_bilags = array();

      foreach ($bpas as $bpa) {
        $bpa = node_load($bpa['target_id']);
        if (empty($bpa->field_os2web_meetings_bpa_file)) {
          $non_bilags[$bpa->nid] = $bpa;
        }
        else {
          $bilags[$bpa->nid] = $bpa;
        }
      }
      if (!empty($non_bilags)) {
        $form['bpas'][] = array (
          '#markup' => '<div  id="js-bpa-non-bilags">',
        );
        foreach ($non_bilags as $bpa) {
          $form['bpas'][] = array(
            '#markup' => '<div class="create-agenda-list-item" id="bpa_' . $bpa->nid . '">',
          );
          $form['bpas'][] = array(
            '#markup' => '<span class="create-agenda-edit-delete-container">',
          );

          $form['bpas'][] = array(
            '#type' => 'image_button',
            '#button_type' => 'submit-edit',
            '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_edit.png',
            '#name' => 'edit_' . $bpa->nid,
            '#submit' => array('os2dagsorden_create_agenda_bullet_point_submit_add_edit_bpa'),
            '#bpa_nid' => $bpa->nid,
          );
          $form['bpas'][] = array(
            '#type' => 'image_button',
            '#button_type' => 'submit-delete',
            '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_delete.png',
            '#name' => 'delete_' . $bpa->nid,
            '#submit' => array('os2dagsorden_create_agenda_bullet_point_submit_delete_bpa'),
            '#bpa_nid' => $bpa->nid,
            '#ajax' => array(
              'callback' => 'os2dagsorden_create_agenda_bpa_delete_ajax',
              'wrapper' => 'js-bpas-fs',
              'method' => 'replace',
              'effect' => 'fade',
            ),
          );
          $form['bpas'][] = array(
          // '<span class="create-agenda-edit-delete-container">',.
            '#markup' => '</span>',
          );

          $form['bpas'][] = array(
            '#markup' => $bpa->title,
          );
          $form['bpas'][] = array(
          // '<div class="create-agenda-list-item">',.
            '#markup' => '</div>',
          );
        }
        $form['bpas'][] = array(
           // '<div class="create-agenda-list-item">',.
             '#markup' => '</div>',
        );
      }
      if (!empty($bilags)) {
        $form['bpas'][] = array(
          '#markup' => '<hr></br>',
        );
        $form['bpas'][] = array (
          '#markup' => '<div  id="js-bpa-bilags">',
        );
        foreach ($bilags as $bpa) {
          $form['bpas'][] = array(
            '#markup' => '<div class="create-agenda-list-item" id="bpa_' . $bpa->nid . '">',
          );
          $form['bpas'][] = array(
            '#markup' => '<span class="create-agenda-edit-delete-container">',
          );

          $form['bpas'][] = array(
            '#type' => 'image_button',
            '#button_type' => 'submit-edit',
            '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_edit.png',
            '#name' => 'edit_' . $bpa->nid,
            '#submit' => array('os2dagsorden_create_agenda_bullet_point_submit_add_edit_bpa'),
            '#bpa_nid' => $bpa->nid,
          );
          $form['bpas'][] = array(
            '#type' => 'image_button',
            '#button_type' => 'submit-delete',
            '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_delete.png',
            '#name' => 'delete_' . $bpa->nid,
            '#submit' => array('os2dagsorden_create_agenda_bullet_point_submit_delete_bpa'),
            '#bpa_nid' => $bpa->nid,
            '#ajax' => array(
              'callback' => 'os2dagsorden_create_agenda_bpa_delete_ajax',
              'wrapper' => 'js-bpas-fs',
              'method' => 'replace',
              'effect' => 'fade',
            ),
          );
          $form['bpas'][] = array(
          // '<span class="create-agenda-edit-delete-container">',.
            '#markup' => '</span>',
          );

          $form['bpas'][] = array(
            '#markup' => $bpa->title,
          );
          $form['bpas'][] = array(
          // '<div class="create-agenda-list-item">',.
            '#markup' => '</div>',
          );
        }
        $form['bpas'][] = array(
          // '<div class="create-agenda-list-item">',.
            '#markup' => '</div>',
        );

      }

    }
  }

  if (!isset($form['bpas'][0])) {
    $form['bpas']['#access'] = FALSE;
  }

  $form['save_bullet_point'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array(
      'class' => array('form-save-submit'),
    ),
    '#validate' => array('os2dagsorden_create_agenda_bullet_point_validate'),
    '#submit' => array('os2dagsorden_create_agenda_bullet_point_submit'),
    '#ajax' => array(
      'callback' => 'os2dagsorden_create_agenda_bullet_point_create_ajax',
      'wrapper' => 'os2dagsorden-create-agenda-meeting',
      'method' => 'replace',
      'effect' => 'fade',
    )
  );

  $form[] = array(
    '#markup' => '</div>',
  );
  $form['#attached']['css'] = array(
     drupal_get_path('module', 'os2dagsorden_create_agenda') . '/css/form_theme.css',
  );
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/js/form_js.js',
  );
  return $form;
}

/**
 * Callback for deleting bullet point attachments.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bpa_delete_ajax($form, $form_state) {
  return $form['bpas'];
}

/**
 * Callback for creating bullet point.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_create_ajax($form, &$form_state) {
  global $base_url;
  if (!empty(form_get_errors())) {
    return $form;
  }
  else {
    $commands = array();
    $commands[] = array(
      'command' => 'closeLightboxAndRedirect',
      'redirect' => $base_url . '/meeting/' . $form_state['meeting_id']
    );
    return array('#type' => 'ajax', '#commands' => $commands);
  }
}

/**
 * Saves the bullet point.
 *
 * Either by creating new or modifying the existing calls
 * os2dagsorden_create_agenda_add_bullet_point_to_meeting.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return object
 *   The node object of bullet point.
 */
function os2dagsorden_create_agenda_save_bullet_point($form, &$form_state) {
  $input = $form_state['input'];

  $title = $input['bp_title'];
  $number = $form_state['values']['number'];
  $closed = $form_state['values']['closed'];
  $personal = $form_state['values']['personal'];
  if (!$form_state['bullet_point_id']) {
    // Node creating.
    $node = new stdClass();
    $node->type = "os2web_meetings_bullet";
    $node->language = LANGUAGE_NONE;
    node_object_prepare($node);
  }
  else {
    $node = node_load($form_state['bullet_point_id']);
  }
  $node->title = $title;
  $node->field_os2web_meetings_bul_number['und'][0]['value'] = $number;
  $node->field_os2web_meetings_bul_closed['und'][0]['value'] = $closed;
  $node->field_os2web_meetings_bul_pers['und'][0]['value'] = $personal;

  node_save($node);
  os2dagsorden_create_agenda_add_bullet_point_to_meeting($form_state['meeting_id'], $node->nid);

  return $node;
}

/**
 * Adds the bullet point to the meeting, if it has not been added before.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 */
function os2dagsorden_create_agenda_add_bullet_point_to_meeting($meeting_id, $bullet_point_id) {
  $meeting = node_load($meeting_id);
  if (field_get_items('node', $meeting, 'field_os2web_meetings_bullets')) {
    $bullet_points = field_get_items('node', $meeting, 'field_os2web_meetings_bullets');
    foreach ($bullet_points as $bp) {
      if ($bp['target_id'] === $bullet_point_id) {
        // No need to readd.
        return;
      }
    }
  }

  $meeting->field_os2web_meetings_bullets['und'][]['target_id'] = $bullet_point_id;
  node_save($meeting);
}

/**
 * Removes the bullet point attachment from the bullet point.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param int $bpa_id
 *   ID of the bullet point attachment.
 */
function os2dagsorden_create_agenda_remove_bpa_from_bullet_point($bullet_point_id, $bpa_id) {
  $bullet_point = node_load($bullet_point_id);
  if (field_get_items('node', $bullet_point, 'field_os2web_meetings_attach')) {
    $bpas = field_get_items('node', $bullet_point, 'field_os2web_meetings_attach');
    foreach ($bpas as $key => $bpa) {
      if ($bpa['target_id'] === $bpa_id) {
        unset($bullet_point->field_os2web_meetings_attach['und'][$key]);
      }
    }
  }

  $bullet_point->field_os2web_meetings_attach['und'] = array_values($bullet_point->field_os2web_meetings_attach['und']);
  node_save($bullet_point);
}

/**
 * Helper function.
 *
 * Bullet point form submit. Calls saving BPA files if exists.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_validate($form, &$form_state) {
  $num_files = count($_FILES['files']['name']);
  for ($i = 0; $i < $num_files; $i++) {
    $file = file_save_upload($i, array(
      'file_validate_extensions' => array('doc docx pdf'),
      'file_validate_size' => array(20 * 1024 * 1024),
    ));
    if ($file) {
      if ($file = file_move($file, 'temporary://', FILE_EXISTS_RENAME)) {
        $form_state['values']['bilags'][$i] = $file;
      }
      else {
        form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
      }
    }
  }
}

/**
 * Helper function.
 *
 * Bullet point form submit. Calls saving and controls the form rebuild (page
 * switching) - returning back to meeting view.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_submit($form, &$form_state) {
  $node = os2dagsorden_create_agenda_save_bullet_point($form, $form_state);
  if (isset($form_state['values']['bilags'])) {
    os2dagsorden_create_agenda_save_bilag_files($node->nid, $form_state);
  }
}

/**
 * Helper functions, bullet point attachment form submit.
 *
 * Calls saving and controls the form rebuild (page switching) redirecting to
 * bullet point attachment creation/editing page.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_submit_add_edit_bpa($form, &$form_state) {
  $node = os2dagsorden_create_agenda_save_bullet_point($form, $form_state);

  $form_state['bullet_point_id'] = $node->nid;
  // If empty - will be treated as new node creation.
  $form_state['bpa_id'] = $form_state['triggering_element']['#bpa_nid'];

  $form_state['page_num'] = 3;
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper functions to delete bullet point attachment.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_submit_delete_bpa($form, &$form_state) {
  $bpa_id = $form_state['triggering_element']['#bpa_nid'];

  os2dagsorden_create_agenda_remove_bpa_from_bullet_point($form_state['bullet_point_id'], $bpa_id);
  node_delete($bpa_id);

  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper functions redirects back to meeting view.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_back($form, &$form_state) {
  if (arg(3) == 'bullet-point') {
    drupal_goto('/meeting/' . $form_state['meeting_id']);
  }
  $form_state['page_num'] = 1;
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper function to generate the number of a next bullet point.
 *
 * @param mixed $meeting_id
 *   The ID of the meeting.
 *
 * @return int
 *   The generated bullet point number.
 */
function os2dagsorden_create_agenda_bullet_point_find_last_number($meeting_id) {
  $number = 1;

  $meeting = node_load($meeting_id);

  if (field_get_items('node', $meeting, 'field_os2web_meetings_bullets')) {
    $bullet_points = field_get_items('node', $meeting, 'field_os2web_meetings_bullets');
    foreach ($bullet_points as $bullet_point_id) {
      $bullet_point = node_load($bullet_point_id['target_id']);
      $current_bp_number = array_pop(field_get_items('node', $bullet_point, 'field_os2web_meetings_bul_number'));
      if ($current_bp_number['value'] > $number) {
        $number = $current_bp_number['value'];
      }
    }

    // Adding 1 to number.
    $number++;
  }

  return $number;
}

/**
 * Helper functions to create bullet point attachment via ajax.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bp_id
 *   ID of the bullet point.
 * @param int $bpa_id
 *   ID of the bullet point attachment.
 */
function os2dagsorden_create_agenda_delete_bpa_ajax_call($meeting_id, $bp_id, $bpa_id) {
  os2dagsorden_create_agenda_remove_bpa_from_bullet_point($bp_id, $bpa_id);
  node_delete($bpa_id);
  drupal_json_output(array('status' => 'OK', 'element' => 'bpa'));
}

/**
 * Saves the bilags.
 *
 * Either by creating new or modifying the existing.
 *
 * @param int $bp_id
 *   The bullet_point ID.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_save_bilag_files($bp_id, $form_state) {
  $bilags = $form_state['values']['bilags'];
  $meeting_id = $form_state['meeting_id'];
  foreach ($bilags as $bilag) {
    if ($bilag['status'] == 'done') {
      $folder_uri = _os2dagsorden_create_agenda_get_meeting_file_folder($meeting_id);
      _os2dagsorden_create_agenda_create_bpa_from_file($bp_id, $bilag, $folder_uri);
    }
  }
}

/**
 * Helper functions to sort bullet point attachment via ajax.
 */
function os2dagsorden_create_agenda_sort_bpa_ajax_call() {
  parse_str($_POST['attachments'], $attachments);
  $bullet_point = node_load($_POST['bullet_point']);
  $bpa_type = $_POST['bpa_type'];
  $attachments = array_pop($attachments);
  $bpa_need_to_be_saved = array();
  foreach($bullet_point->field_os2web_meetings_attach['und'] as $bpa) {
    if (!in_array($bpa['target_id'], $attachments)){
      $bpa_need_to_be_saved[] = $bpa;
    }
  }
  unset($bullet_point->field_os2web_meetings_attach['und']);
  $sorted_bpa = array();
  foreach ($attachments as $nid) {
   $sorted_bpa[] = array('target_id' => $nid);
  }
  if ($bpa_type == 'bilag') {
    $bullet_point->field_os2web_meetings_attach['und'] = array_merge($bpa_need_to_be_saved, $sorted_bpa);
  }
  else {
    $bullet_point->field_os2web_meetings_attach['und'] = array_merge($sorted_bpa, $bpa_need_to_be_saved);
  }
  node_save($bullet_point);
}

/**
 * Generates the form for creating/modifying bullet points and attachments from  .
 *
 * @param mixed $form
 *   The form to be created.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return mixed
 *   The created form.
 */
function os2dagsorden_create_agenda_bullet_point_from_file_form($form, &$form_state) {
  $form[] = array(
    '#markup' => '<h1 class="title">' . t('Bullet point') . '</h1>',
  );

  $form[] = array(
    '#markup' => '<div class="node">',
  );
  $form['meeting_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(2)
  );
  $form['create_as_bpa'] = array(
    '#type' => 'checkbox',
    '#title' => t('Opret som underpunkter')
  );
  $max_filesize_bytes = variable_get('os2dagsorden_plupupload_max_filesize', 2) * 1024 * 1024;
  $allowed_extensions = variable_get('os2dagsorden_plupupload_allowed_filetypes', 'doc docx pdf');

  $form['bilags'] = array(
    '#type' => 'plupload',
    '#name' => 'files[]',
    '#title' => t('Upload bilags files'),
    '#autoupload' => variable_get('os2dagsorden_plupupload_autoupload', TRUE),
    '#upload_validators' => array(
      'file_validate_extensions' => array($allowed_extensions),
      'file_validate_size' => array($max_filesize_bytes),
    )
  );
  if (variable_get('os2dagsorden_plupupload_autoupload', TRUE)) {
    $form['bilags']['#prefix'] = '<div class="field-autoupload">';
    $form['bilags']['#suffix'] = '</div>';
  }
  $form['save_bullet_point'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#attributes' => array(
      'class' => array('form-save-submit'),
    ),
    '#submit' => array('os2dagsorden_create_agenda_bullet_point_from_file_form_submit'),
    '#ajax' => array(
      'callback' => 'os2dagsorden_create_agenda_bullet_point_create_ajax',
      'wrapper' => 'os2dagsorden-create-agenda-meeting',
      'method' => 'replace',
      'effect' => 'fade',
    )
  );
  $form[] = array(
    '#markup' => '</div>',
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/css/form_theme.css',
  );
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/js/form_js.js',
  );
  return $form;
}

/**
 * Form submit function.
 * Created bullet points and attachments from files.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_from_file_form_submit($form, &$form_state) {
  $bilags = $form_state['values']['bilags'];
  $meeting_id = $form_state['values']['meeting_id'];
  $create_as_bpa = $form_state['values']['create_as_bpa'];
  $form_state['meeting_id'] = $meeting_id;
  if (isset($bilags) && !empty($bilags)) {
    $file_folder_uri = _os2dagsorden_create_agenda_get_meeting_file_folder($meeting_id);
    if ($create_as_bpa == 1) {
      $number = os2dagsorden_create_agenda_bullet_point_find_last_number($meeting_id);
      $bp_node = new stdClass();
      $bp_node->type = "os2web_meetings_bullet";
      $bp_node->language = LANGUAGE_NONE;
      node_object_prepare($bp_node);
      $bp_node->title = 'Punkt ' . $number . ' ' . t('Nyt punkt');
      $bp_node->field_os2web_meetings_bul_number['und'][0]['value'] = $number;
      node_save($bp_node);
      os2dagsorden_create_agenda_add_bullet_point_to_meeting($meeting_id, $bp_node->nid);
      foreach ($bilags as $bilag) {
       _os2dagsorden_create_agenda_create_bpa_from_file($bp_node->nid, $bilag, $file_folder_uri);
      }
    }
    else {
      foreach ($bilags as $bilag) {
        if ($bilag['status'] === 'done') {
          $number = os2dagsorden_create_agenda_bullet_point_find_last_number($meeting_id);
          $bp_node = new stdClass();
          $bp_node->type = "os2web_meetings_bullet";
          $bp_node->language = LANGUAGE_NONE;
          node_object_prepare($bp_node);
          $bp_node->title = 'Punkt ' . $number . ' ' . pathinfo($bilag['name'], PATHINFO_FILENAME);
          $bp_node->field_os2web_meetings_bul_number['und'][0]['value'] = $number;
          node_save($bp_node);
          os2dagsorden_create_agenda_add_bullet_point_to_meeting($meeting_id, $bp_node->nid);
          _os2dagsorden_create_agenda_create_bpa_from_file($bp_node->nid, $bilag, $file_folder_uri);
        }
      }
    }
  }
  return;
}

/**
 * Helper functions to create bullet point attachment from file.
 *
 * @param int $bilag_id
 *   The form.
 * @param mixed $bilag_array
 *   The array from plupupload widget.
 * @param string $folder_uri
 *   URI to meeting's files folder.
 */
function _os2dagsorden_create_agenda_create_bpa_from_file($bp_id, $bilag_array, $folder_uri) {
  $source = $bilag_array['tmppath'];
  if (file_prepare_directory($folder_uri, FILE_CREATE_DIRECTORY)){
      $destination = file_stream_wrapper_uri_normalize($folder_uri . '/' . $bilag_array['name']);
      $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);
      $bilag = plupload_file_uri_to_object($destination);
      file_save($bilag);
      $node = new stdClass();
      $node->type = "os2web_meetings_bullet_attach";
      $node->language = LANGUAGE_NONE;
      node_object_prepare($node);
      $node->title = pathinfo($bilag->filename, PATHINFO_FILENAME);
      $bilag->status = FILE_STATUS_PERMANENT;
      $bilag->display = 1;
      file_save($bilag);
      $node->field_os2web_meetings_bpa_file['und'][0] = (array) $bilag;
      node_save($node);
      os2dagsorden_create_agenda_add_bpa_to_bullet_point($bp_id, $node->nid);
      if ($bilag) {
        if (module_exists("os2dagsorden_pdf2htmlex")) {
          // If is PDF, schedule for HTML conversion.
          if (mime_content_type(drupal_realpath($bilag->uri)) === 'application/pdf') {
            os2dagsorden_pdf2htmlex_schedule($bilag->fid, $node->nid);
          }
        }
        if (module_exists('os2web_pdf_conversion_manager')) {
          if (mime_content_type(drupal_realpath($bilag->uri)) !== 'application/pdf') {
           _os2web_pdf_conversion_manager_copy_file_to_convertion_dir($bilag->fid);
          }
        }
        // PNG  conversion.
        if (module_exists("os2dagsorden_pdf2png_conversion")) {
          // If is PDF, schedule for HTML conversion.
          if (mime_content_type(drupal_realpath($bilag->uri)) === 'application/pdf') {
            os2dagsorden_pdf2png_conversion_schedule_if_bpa($bilag->fid);
          }
        }
      }
  }
  else {
    watchdog('os2dagsorden_create_agenda', 'Folder %uri can\'t be created.', array('%uri' => $folder_uri), WATCHDOG_ERROR);
  }

}
