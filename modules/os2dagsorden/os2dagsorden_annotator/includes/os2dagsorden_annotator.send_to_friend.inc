<?php

/**
 * Creates a form.
 *
 * Creates a form that allows to send the meetings notes to an
 * email addess.
 *
 * @param int $meeting_id
 *   Meeting id.
 */
function os2dagsorden_annotator_send_notes_to_friend_form($meeting_id) {
  // Adding css that tweaks the elements for the modal window.
  drupal_add_css(drupal_get_path('module', 'os2web_print_send_to_friend') . '/css/os2web_print_send_to_friend.css');
  drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/js/os2dagsorden_annotator_send_to_friend.js');

  $confirm_popup = '';

  $html = t('Udfyld felterne herunder og tryk "Send", når du vil sende beskeden.');
  $html .= '<form id="send_to_friend_form">';
  $html .= '<label>' . t('Dit navn:') . '</label><input type="text" name="name" />';
  $html .= '<label>' . t('Besked:') . '</label><textarea name="message"></textarea>';
  $html .= '<label>' . t('Modtagers email:') . '</label><input type="text" name="email" id="field_send_to_friend_email" />';
  $html .= '<input type="hidden" name="base_url" id="field_base_url" value="' . $GLOBALS['base_url'] . '" />';
  $html .= '<input type="hidden" name="meeting_id" id="field_meeting_id" value="' . $meeting_id . '" />';
  $html .= '<button type="submit" class="form-submit" id="btn_send_to_friend"' . $confirm_popup . '>Send</button>';
  $html .= '<div class="throbber"></div>';
  $html .= '</form>';

  return '<div class="node"><div class="bullet-point-send-to-friend-form ajax-progress">' . $html . '</div></div>';
}

/**
 * Does the actual sending to the provided email.
 *
 * @param int $meeting_id
 *   Meeting id.
 */
function os2dagsorden_annotator_send_notes_to_friend($meeting_id) {
  $meetings = node_load($meeting_id);
  $subject = 'Møder noter: ' . $meetings->title;

  $name = check_plain(trim($_POST['name']));
  $message = check_plain(trim($_POST['message']));
  $to_email = check_plain($_POST['email']);

  $body = "<html><body>";

  $body .= $name . " har sendt dig møder noter med beskeden:<br/>" . $message . "<br/><br/>";

  $body .= _os2dagsorden_annotator_meeting_all_notes_generate_html($meeting_id);

  $body .= "</body></html>";

  $from = variable_get('system_mail');
  $params = array(
    'subject' => $subject,
    'body' => $body,
  );

  drupal_mail('os2web_print_send_to_friend', 'send_to_friend_', $to_email, language_default(), $params, $from);
}
