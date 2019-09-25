<?php

/**
 * Generates the form for user creating.
 *
 * @param mixed $form
 *   The form to be created.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return mixed
 *   The created form.
 */
function os2dagsorden_create_agenda_meeting_create_user_form($form, &$form_state) {
  $form_state['meeting_data'] = json_encode($form_state['values']);
  $form[] = array(
    '#markup' => '<h1 class="title">' . t('External User') . '</h1>',
  );

  $form[] = array(
    '#markup' => '<div class="node">',
  );
  $form['firstname'] =  array(
    '#type' => 'textfield',
    '#title' => t('Firstname'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['lastname'] =  array(
    '#type' => 'textfield',
    '#title' => t('Lastname'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
  );
  $form['email'] =  array(
    '#type' => 'textfield',
    '#title' => t('Email'),
     '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#element_validate' => array('_os2dagsorden_create_agenda_meeting_create_user_form_email_validate'),
  );
  $form['save_bullet_point'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#submit' => array('_os2dagsorden_create_agenda_meeting_create_user_form_submit'),
  );
  $form[] = array(
    '#markup' => '</div>',
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/css/form_theme.css',
  );
  return $form;
}

/**
 * Helper function.
 *
 * Email validation.
 *
 * @param mixed $element
 *   The element.
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function _os2dagsorden_create_agenda_meeting_create_user_form_email_validate($element, $form, &$form_state) {
  $value = $element['#value'];
  if (!valid_email_address($value)) {
    form_error($element, t('Please enter a valid email address.'));
  }
}
/**
 * Helper functions to save user.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function _os2dagsorden_create_agenda_meeting_create_user_form_submit($form, &$form_state) {
  // Create user.
  $values = $form_state['values'];
  $email = $values['email'];
  $full_name = $values['firstname'] . ' ' . $values['lastname'];
  $password = user_password(8);
  $fields = array(
    'name' => $email,
    'pass' => $password,
    'mail' => $email,
    'status' => 1,
    'init' => 'email address',
    'field_user_full_name' => array(LANGUAGE_NONE => array(array('value' => $full_name))),
    'field_user_external' => array(LANGUAGE_NONE => array(array('value' => 1))),
  );
  $account = user_save('', $fields);
  // Send notification email.
  drupal_mail('user', 'password_reset', $email, NULL, array('account' => $account), variable_get('site_mail', ''));

  $form_state['input'] = (array) json_decode($form_state['meeting_data']);
  $participants = explode(',', $form_state['input']['participants_hidden']);
  $participants[] = $account->uid;
  $participants = array_diff($participants, array(''));
  $form_state['input']['participants_hidden'] = implode(',', $participants);
  if (isset($form_state['input']['start_date'])) {
    $timestamp = strtotime($form_state['input']['start_date']);
    $form_state['input']['start_date'] = array (
      'date' => date('d/m/Y', $timestamp),
      'time' => date('H:i', $timestamp)
    );
  }
  $form_state['page_num'] = 1;
  $form_state['rebuild'] = TRUE;

}
