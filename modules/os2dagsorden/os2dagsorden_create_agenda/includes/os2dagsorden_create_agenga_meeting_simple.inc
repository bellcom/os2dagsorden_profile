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
 * Generates form for meeting simple edit.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The state of the form.
 * @param int $meeting_id
 *   ID of the meeting.
 *
 * @return mixed
 *   The form.
 */
function os2dagsorden_create_agenda_meeting_simple($form, &$form_state, $meeting_id) {
  $form[] = array(
    '#markup' => '<h1 class="title">' . t('Meeting') . '</h1>',
  );

  // Adding general fields.
  $form = os2dagsorden_create_agenda_meeting_form_add_general_fields($form, $form_state);

  // Filling the fields if meeting_id is not empty.
  if (!$meeting_id) {
    // If meeting id is empty, try to fill it with form_state meeting_id value.
    $meeting_id = $form_state['meeting_id'];
  }
  _os2dagsorden_create_agenda_render_drag_and_drop_controls($form, $form_state, $meeting_id);
  if ($meeting_id) {
    // Saving for usage in the submit function.
    $form_state['meeting_id'] = $meeting_id;

    $node = node_load($meeting_id);

    // Filling general fields.
    $form = os2dagsorden_create_agenda_meeting_form_fill_general_fields($form, $node);
  }

  // Finishing form.
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/css/form_theme.css',
  );

  $form['save_meeting'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-save-submit'),
    ),
    '#value' => t('Save meeting'),
    '#submit' => array('os2dagsorden_create_agenda_meeting_simple_submit'),
  );
  $form[] = array(
    '#markup' => '</div>',
  );

  return $form;
}

/**
 * Saves the meeting from the submitted form.
 *
 * @param mixed $form
 *   The form where information is taken from.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return object
 *   Populated but unsaved(!) node.
 */
function os2dagsorden_create_agenda_save_meeting_simple($form, $form_state) {
  $node = os2dagsorden_create_agenda_populate_meeting_general($form, $form_state);

  $input = $form_state['input'];
  $node->field_os2web_meetings_type['und'][0]['value'] = $input['status'];
  node_submit($node);
  node_save($node);

  return $node;
}

/**
 * Meeting simple form submit callback.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The state of the form.
 */
function os2dagsorden_create_agenda_meeting_simple_submit($form, &$form_state) {
  $node = os2dagsorden_create_agenda_save_meeting_simple($form, $form_state);

  $form_state['redirect'] = 'meeting/' . $node->nid;
}
