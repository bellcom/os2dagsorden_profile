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
 * Generates the form for bullet point attachment creating/modifying.
 *
 * @param mixed $form
 *   The form to be created.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return mixed
 *   The created form.
 */
function os2dagsorden_create_agenda_bpa($form, &$form_state) {
  $form[] = array(
    '#markup' => '<h1 class="title">' . t('Bullet point attachment') . '</h1>',
  );

  $form[] = array(
    '#markup' => '<div class="node">',
  );

  $form['bpa_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title:'),
    '#required' => TRUE,
  );

  $form['add_attachment'] = array(
    '#type' => 'vertical_tabs',
  );

  $form['attachment'] = array(
    '#type' => 'fieldset',
    '#title' => t('Attachment (text)'),
    '#collapsible' => TRUE,
    '#description' => t('Add the text-based bullet point attachment to the bullet point.'),
    '#group' => 'add_attachment',
  );

  $form['attachment']['body'] = array(
    '#type' => 'text_format',
    '#title' => t('Body:'),
    '#format' => 'full_html',
    '#group' => 'add_attachment',
  );

  $form['bilag'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bilag (file)'),
    '#collapsible' => TRUE,
    '#description' => t('Enclose a file to the bullet point attachment.'),
    '#group' => 'add_attachment',

  );
  if ($form_state['bpa_id']) {
    $node = node_load($form_state['bpa_id']);
    if (field_get_items('node', $node, 'field_os2web_meetings_bpa_file')) {
      $bilag = field_get_items('node', $node, 'field_os2web_meetings_attach');
      $bilag = array_pop(field_get_items('node', $node, 'field_os2web_meetings_bpa_file'));
      $file = file_load($bilag['fid']);
      $form['bilag']['bpa_file'][] = array(
          '#markup' => '<div class="create-agenda-list-item" id="bpa_file_' . $bilag['fid'] . '">',
        );

       $form['bilag']['bpa_file'][] = array(
          '#markup' => '<span class="create-agenda-edit-delete-container">',
        );


        $form['bilag']['bpa_file'][] = array(
          '#type' => 'image_button',
          '#button_type' => 'submit-delete',
          '#src' => drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/icon_delete.png',
          '#name' => 'delete_' . $bilag['fid'],
          '#submit' => array('os2dagsorden_create_agenda_bullet_point_submit_delete_bpa_file'),
          '#bpa_fid' => $bilag['fid'],
          '#bpa_nid' => $node->nid,
          '#ajax' => array(
            'callback' => 'os2dagsorden_create_agenda_bpa_file_delete_ajax',
            'wrapper' => 'bbpa_file_' . $bilag['fid'],
            'method' => 'replace',
            'effect' => 'fade',
          ),
        );

        $form['bilag']['bpa_file'][] = array(
        // '<span class="create-agenda-edit-delete-container">',.
          '#markup' => '</span>',
        );

        $form['bilag']['bpa_file'][] = array(
          '#markup' => l($file->filename, file_create_url($file->uri))
        );

        $form['bilag']['bpa_file'][] = array(
        // '<div class="create-agenda-list-item">',.
          '#markup' => '</div>',
        );
      }
    }
  // Pass the maximum file size in bytes - 20 MB.
  $max_filesize_bytes = variable_get('os2dagsorden_plupupload_max_filesize', 2) * 1024 * 1024;
  $allowed_extensions = variable_get('os2dagsorden_plupupload_allowed_filetypes', 'doc docx pdf');

  $form['bilag']['file'] = array(
    '#type' => 'plupload',
    '#title' => t('Bilag file:'),
    '#group' => 'add_attachment',
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

  // Filling the fields if meeting_id is not empty.
  if ($form_state['bpa_id']) {
    $node = node_load($form_state['bpa_id']);
    $form['bpa_title']['#default_value'] = $node->title;

    if (field_get_items('node', $node, 'field_os2web_meetings_bpa_body')) {
      $body_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_bpa_body'));
      $form['attachment']['body']['#default_value'] = $body_field['value'];
      $form['add_attachment']['#default_tab'] = 'edit-attachment';
    }

    if (field_get_items('node', $node, 'field_os2web_meetings_bpa_file')) {
      $file_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_bpa_file'));
      $form['bilag']['file']['#default_value'] = $file_field['fid'];
      $form['add_attachment']['#default_tab'] = 'edit-bilag';
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => (arg(5) == 'attachment') ? t('Save') : t('Save & return to bullet point'),
    '#attributes' => array(
      'class' => array('form-save-submit'),
    ),
    '#validate' => array('os2dagsorden_create_agenda_bpa_validate'),
    '#submit' => array('os2dagsorden_create_agenda_bpa_submit'),
  );
  if (arg(5) == 'attachment') {
    $form['submit']['#ajax'] = array(
      'callback' => 'os2dagsorden_create_agenda_bpa_create_ajax',
      'wrapper' => 'os2dagsorden-create-agenda-meeting',
      'method' => 'replace',
      'effect' => 'fade',
    );
  }

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
 * Custom validation of bullet point attachment form.
 *
 * Validates that only one of the option is chosen  - either file is uploaded or
 * body of the bullet point attachmet is filled.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bpa_validate($form, &$form_state) {
  $input = $form_state['input'];

  $body = $input['body']['value'];
  $bilag = $form_state['values']['file'];
  if (!empty($body) && $bilag != 0) {
    form_set_error('body', t('You cannot provide both file and the body text. Please choose one of the options.'));
  }
}

/**
 * Saves the bullet point attachment.
 *
 * Either by creating new or modifying the existing. Performs call to
 * os2dagsorden_create_agenda_add_bpa_to_bullet_point.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return object
 *   Bullet point attachment.
 */
function os2dagsorden_create_agenda_save_bpa($form, &$form_state) {
  $input = $form_state['input'];

  $title = $input['bpa_title'];
  $body = $input['body']['value'];
  $bilag = $form_state['values']['file'];

  if (!$form_state['bpa_id']) {
    // Node creating.
    $node = new stdClass();
    $node->type = "os2web_meetings_bullet_attach";
    $node->language = LANGUAGE_NONE;
    node_object_prepare($node);
  }
  else {
    $node = node_load($form_state['bpa_id']);
  }

  $node->title = $title;
  $node->field_os2web_meetings_bpa_body['und'][0]['value'] = $body;
  if (!empty($bilag)) {
    foreach ($bilag as $bilag_file) {
      if ($bilag_file['status'] == 'done') {
        // Resaving file permanently.
        $source = $bilag_file['tmppath'];
        $destination = file_stream_wrapper_uri_normalize(variable_get('os2dagsorden_plupupload_filedir_uri', 'private://manual_agendas') . '/'. $bilag_file['name']);
        $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);
        $file = plupload_file_uri_to_object($destination);
        file_save($file);
        $file->status = FILE_STATUS_PERMANENT;
        $file->display = 1;
        file_save($file);
        $node->field_os2web_meetings_bpa_file['und'][0] = (array) $file;
        break;
      }
    }
  }
  node_save($node);

  os2dagsorden_create_agenda_add_bpa_to_bullet_point($form_state['bullet_point_id'], $node->nid);

  // Schedule file for html conversion.
  if ($bilag) {
    os2dagsorden_pdf2htmlex_schedule_if_bpa($bilag->fid);
    // PNG  conversion.
    if (module_exists("os2dagsorden_pdf2png_conversion")) {
      // If is PDF, schedule for HTML conversion.
      os2dagsorden_pdf2png_conversion_schedule_if_bpa($file->fid);
    }
  }

  return $node;
}

/**
 * Adds the bullet points attachment to the bullet point.
 *
 * If it has not been added before.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param int $bpa_id
 *   ID of the bullet point attachment.
 */
function os2dagsorden_create_agenda_add_bpa_to_bullet_point($bullet_point_id, $bpa_id) {
  $bullet_point = node_load($bullet_point_id);
  if (field_get_items('node', $bullet_point, 'field_os2web_meetings_attach')) {
    $bpas = field_get_items('node', $bullet_point, 'field_os2web_meetings_attach');
    foreach ($bpas as $bpa) {
      if ($bpa['target_id'] === $bpa_id) {
        // No need to readd.
        return;
      }
    }
  }

  $bullet_point->field_os2web_meetings_attach['und'][]['target_id'] = $bpa_id;
  node_save($bullet_point);
}

/**
 * Helper functions, bullet point attachment form submit.
 *
 * Calls os2dagsorden_create_agenda_save_bpa returns to the previous page of
 * multiform (bullet point creation/modification).
 *
 * @param mixed $form
 *   The form to be created.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bpa_submit($form, &$form_state) {
  os2dagsorden_create_agenda_save_bpa($form, $form_state);
  if (arg(5) == 'attachment') {
    drupal_goto('/meeting/' . $form_state['meeting_id']);
  }
  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

/**
 * Helper functions.
 *
 * Returns to the previous page of multiform (bullet point creation/
 * modification).
 *
 * @param mixed $form
 *   The form to be created.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bpa_back($form, &$form_state) {
  if (arg(5) == 'attachment') {
    drupal_goto('/meeting/' . $form_state['meeting_id']);
  }
  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

/**
 * Callback for creating bullet point attachment.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bpa_create_ajax($form, &$form_state) {
  if (!empty(form_get_errors())) {
    return $form;
  }
  else {
    $commands = array();
    $commands[] = array('command' => 'closeLightboxPopup');
    return array('#type' => 'ajax', '#commands' => $commands);
  }
}

/**
 * Helper functions to delete bullet point attachment file.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bullet_point_submit_delete_bpa_file($form, &$form_state) {
  $fid = $form_state['triggering_element']['#bpa_fid'];
  $nid = $form_state['triggering_element']['#bpa_nid'];
  $to_del_file = file_load($fid);
  $node = node_load($nid);
  unset($node->field_os2web_meetings_bpa_file['und']);
  file_delete($to_del_file);
  node_save($node);
  $form_state['page_num'] = 3;
  $form_state['rebuild'] = TRUE;
}

/**
 * Callback for deleting bullet point attachment files.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_bpa_file_delete_ajax($form, $form_state) {
  return '';
}