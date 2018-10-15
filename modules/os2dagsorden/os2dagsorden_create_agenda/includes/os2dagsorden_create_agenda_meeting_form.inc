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
 * Add to the form the field.
 *
 * Used both for simple and full agenda meeting creation.
 *
 * @param mixed $form
 *   The form which should be modified.
 * @param mixed $form_state
 *   The form_state.
 *
 * @return mixed
 *   The form with new field.
 */
function os2dagsorden_create_agenda_meeting_form_add_general_fields($form, $form_state) {
  $form[] = array(
    '#markup' => '<div class="node">',
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#attributes' => array('required' => 'required'),
    '#description' => t('Full title of the meeting'),
  );

  $form['status'] = array(
    '#type' => 'select',
    '#title' => t('Status:'),
    '#required' => TRUE,
    '#options' => array(
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PLANNED => t('Planlagt'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_CHANGED => t('Ændret'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_CANCELLED => t('Aflyst'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_REFERAT => t('Referat'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_DAGSORDEN => t('Dagsorden'),
    ),
    '#multiple' => FALSE,
    '#default_value' => OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PLANNED,
  );
  $form['hex_color'] = array(
    '#type' => 'jquery_colorpicker',
    '#title' => t('Meeting color:'),
    '#default_value' => '757575',
    '#description' => t('Choose a color for meeting, will be used to for marking the meeting in the calendar'),
  );
  $form[] = array(
    '#markup' => '<div class="clearfix">',
  );
  $form['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start Date:'),
    '#size' => 10,
    '#required' => TRUE,
    '#date_format' => 'd/m/Y H:i',
    '#attributes' => array('required' => 'required'),
    '#ajax' => array(
      'wrapper' => 'js-end-date-wrapper',
      'callback' => 'os2dagsorden_create_agenda_populate_end_date',
      'method' => 'replace',
    )
  );
  $form['end_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('End Date:'),
    '#size' => 10,
    '#required' => FALSE,
    '#date_format' => 'd/m/Y H:i',
    '#prefix' => '<div id="js-end-date-wrapper">',
    '#suffix' => '</div>',
  );

  if ($form_state['input']['start_date']['date']) {
    $form['end_date']['#value']['date'] = $form_state['input']['start_date']['date'];
  }
  if ($form_state['input']['start_date']['time']) {
    $form['end_date']['#value']['time'] = $form_state['input']['start_date']['time'];
  }

  $form[] = array(
    '#markup' => '</div>',
  );

  $loc_voc = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_location');
  $loc_terms = taxonomy_get_tree($loc_voc->vid);
  $loc_select = array();
  foreach ($loc_terms as $loc_term) {
    $loc_select[$loc_term->tid] = $loc_term->name;
  }
  $form['location'] = array(
    '#type' => 'select',
    '#title' => t('Location:'),
    '#required' => FALSE,
    '#options' => $loc_select,
    '#multiple' => FALSE,
    '#empty_value' => 0,
  );

  $com_voc = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_committee');
  $com_terms = taxonomy_get_tree($com_voc->vid);
  $com_select = array();
  foreach ($com_terms as $com_term) {
    if (os2dagsorden_access_helper_check_user_can_create_meeting_in_committee($com_term->tid)) {
      $com_select[$com_term->tid] = $com_term->name;
    }
  }
  $form['os2web_meetings_tax_committee'] = array(
    '#type' => 'select',
    '#title' => t('Committee:'),
    '#required' => TRUE,
    '#options' => $com_select,
    '#attributes' => array('required' => 'required'),
    '#multiple' => FALSE,
  );

 /* $user_select = array();
  $users = entity_load('user');
  foreach ($users as $user) {
    if ((int)$user->uid === 0) {
      continue;
    }
    $user_select[$user->uid] = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
  }
  $form['participants'] = array(
    '#type' => 'select',
    '#title' => t('Participants:'),
    '#required' => FALSE,
    '#options' => $user_select,
    '#multiple' => TRUE,
    '#attributes' => array('class' => array('improved-multi-select')),
  );

   $form['referents'] = array(
    '#type' => 'select',
    '#title' => t('Referent:'),
    '#required' => FALSE,
    '#options' => $user_select,
    '#multiple' => TRUE,
    '#attributes' => array('class' => array('improved-multi-select')),
  );
*/

  // Finishing form.
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'os2dagsorden_create_agenda') . '/css/form_theme.css',
  );
  return $form;
}

/**
 * Fill the general fields of the form with the values from meeting.
 *
 * Both used for simple and full agenda meeting creation.
 *
 * @param mixed $form
 *   The form which should be filled.
 * @param object $node
 *   The meeting node, where the information should be taken from.
 *
 * @return mixed
 *   The form with filled fields.
 */
function os2dagsorden_create_agenda_meeting_form_fill_general_fields($form, $node) {
  $form['title']['#default_value'] = $node->title;

  $status_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_type'));
  $form['status']['#default_value'] = $status_field['value'];

  if (field_get_items('node', $node, 'field_os2web_meetings_hex')) {
    $hex_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_hex'));
    $form['hex_color']['#default_value'] = $hex_field['safe_value'];
  }

  $date_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_date'));
  $form['start_date']['#default_value'] = $date_field['value'];
  if ($date_field['value2'] != $date_field['value']) {
    $form['end_date']['#default_value'] = $date_field['value2'];
  }

  $committee_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_committee'));
  $form['os2web_meetings_tax_committee']['#default_value'] = $committee_field['tid'];

  if (field_get_items('node', $node, 'field_os2web_meetings_location')) {
    $location_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_location'));
    $form['location']['#default_value'] = $location_field['tid'];
  }

  if (field_get_items('node', $node, 'field_os2web_meetings_referent')) {
    $referent_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_referent'));
    $referents = explode(',', $referent_field['safe_value']);
    foreach ($referents as $el => $value) {
      // Array.
      $value = explode(' ', $value);
      // (name)
      $value = end($value);
      // Name.
      $username = substr($value, 1, strlen($value) - 2);
       $account = user_load_by_name($username);
      $selection[] = $account->uid;
    }
    $form['referents']['#default_value'] = $selection;
  }

  if (field_get_items('node', $node, 'field_os2web_meetings_partic')) {
    $participants_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_partic'));
    $participants = explode(',', $participants_field['safe_value']);
    foreach ($participants as $el => $value) {
      // Array.
      $value = explode(' ', $value);
      // (name)
      $value = end($value);
      // Name.
      $username = substr($value, 1, strlen($value) - 2);
      $account = user_load_by_name($username);
      $selection[] = $account->uid;
    }
    $form['participants']['#default_value'] = $selection;
  }

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
function os2dagsorden_create_agenda_populate_end_date($form, &$form_state) {
  if ($form_state['input']['start_date']['date']) {
    $form['end_date']['#value']['date'] = $form_state['input']['start_date']['date'];
  }
  if ($form_state['input']['start_date']['time']) {
    $form['end_date']['#value']['time'] = $form_state['input']['start_date']['time'];
  }

  return $form['end_date'];
}

/**
 * Populates the meeting node.
 *
 * Either new or existing depending on the content with the information taken
 * from the form.
 *
 * @param mixed $form
 *   The form where information is taken from.
 * @param mixed $form_state
 *   The current state of the form.
 *
 * @return object
 *   Populated but unsaved(!) node.
 */
function os2dagsorden_create_agenda_populate_meeting_general($form, $form_state) {
  $input = $form_state['input'];

  // Input extraction.
  $title = $input['title'];
  $status = $input['status'];
  $hex_color = $input['hex_color'];
  $start_date = $input['start_date']['date'];
  $start_time = $input['start_date']['time'];
  $end_date = $input['end_date']['date'];
  $end_time = $input['end_date']['time'];
  $loc_tid = $input['location'];
  $com_tid = $input['os2web_meetings_tax_committee'];
  $participants = $input['participants'];
  $referents = $input['referents'];

  $participants = $input['participants'];
  $internal_participants = explode(',', $input['internal_participants_hidden']);
  $external_participants = explode(',', $input['external_participants_hidden']);
  $invisible_participants = explode(',', $input['participants_invisible_hidden']);
  if (!$form_state['meeting_id']) {
    // Node creating.
    $node = new stdClass();
    $node->type = "os2web_meetings_meeting";
    $node->language = LANGUAGE_NONE;
    node_object_prepare($node);
  }
  else {
    $node = node_load($form_state['meeting_id']);
  }

  // Node population.
  $node->title = $title;
  $node->field_os2web_meetings_type['und'][0]['value'] = $status;
  $start_datetime = date_create_from_format("d/m/Y H:i", $start_date . ' ' . $start_time);
  $node->field_os2web_meetings_date['und'][0]['value'] = $start_datetime->format('Y-m-d H:i:s');
  $node->field_os2web_meetings_committee['und'][0]['tid'] = $com_tid;

  $node->field_os2web_meetings_hex['und'][0]['value'] = $hex_color;

  if (!empty($end_date)) {
    $end_datetime = date_create_from_format("d/m/Y H:i", $end_date . ' ' . $end_time);
    $node->field_os2web_meetings_date['und'][0]['value2'] = $end_datetime->format('Y-m-d H:i:s');
    // Same as start date - default.
  }
  else {
    $node->field_os2web_meetings_date['und'][0]['value2'] = $start_datetime->format('Y-m-d H:i:s');
  }

  $node->field_os2web_meetings_location['und'][0]['tid'] = $loc_tid;

  /*if ($participants) {
    $participant_str = '';
    foreach ($participants as $participant => $value) {
      $user = user_load($value);
      $participant_str .= $user->field_user_full_name['und'][0]['value'] . " ($user->name), ";
    }
    // Removing last coma.
    $node->field_os2web_meetings_partic['und'][0]['value'] = substr($participant_str, 0, strlen($participant_str) - 2);
  }
  else {
    $node->field_os2web_meetings_partic['und'][0]['value'] = '';
  }*/
  // Unsetting internal members.
  $node->field_os2web_meetings_partic_int['und'] = array();
  if ($internal_participants) {
    foreach ($internal_participants as $uid) {
      if (!empty($uid)) {
        $node->field_os2web_meetings_partic_int['und'][]['uid'] = $uid;
      }
    }
  }
  // Unsetting external members.
  $node->field_os2web_meetings_partic_ext['und'] = array();
  if ($external_participants) {
    $role = user_role_load_by_name("os2dagsorden limited");
    foreach ($external_participants as $uid) {
      if (!empty($uid)) {
        $node->field_os2web_meetings_partic_ext['und'][]['uid'] = $uid;
      }
    }
    // Add os2dagsorden limited role.
    user_multiple_role_edit($external_participants, 'add_role', $role->rid);
  }
  // Unsetting external members.
  $node->field_os2web_meetings_partic_inv['und'] = array();
  if ($invisible_participants) {
    foreach ($invisible_participants as $uid) {
      if (!empty($uid)) {
        $node->field_os2web_meetings_partic_inv['und'][]['uid'] = $uid;
      }
    }
  }

  $participants = array_merge($external_participants, $internal_participants);
  foreach ($participants as $uid) {
    if (!empty($uid)) {
      $user = user_load($uid);
      // Member of.
      $is_member_of = FALSE;
      if ($user_member_of_field = field_get_items('user', $user, 'field_user_committee')) {

        foreach ($user_member_of_field as $member_of) {
          if ((int) $com_tid === (int) $member_of['tid']) {
            $is_member_of = TRUE;
            break;
          }
        }
      }
      if (!$is_member_of) {
        $user->field_user_committee['und'][]['tid'] = $com_tid;
        user_save($user);
      }
    }
  }
  if ($referents) {
    $referent_str = '';
    foreach ($referents as $referent => $value) {
      $user = user_load($value);
      $referent_str .= $user->field_user_full_name['und'][0]['value'] . " ($user->name), ";
    }
    // Removing last coma.
    $node->field_os2web_meetings_referent['und'][0]['value'] = substr($referent_str, 0, strlen($referent_str) - 2);
  }
  else {
    $node->field_os2web_meetings_referent['und'][0]['value'] = '';
  }

  return $node;
}

/**
 * Checks is current user is committee admin .
 *
 * @param int $committee_tid
 *   The committee ID.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_create_agenda_is_user_committee_admin($committee_tid) {
  $user = os2dagsorden_access_helper_get_user();
  $committee = taxonomy_term_load($committee_tid);
  $committee_admins = field_get_items('taxonomy_term', $committee, 'field_os2web_meetings_com_admin');
  if ($committee_admins) {
    foreach ($committee_admins as $key => $admin) {
      if ($user->uid === $admin['uid']) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Fills forms with drag and drop controls for manipulating participants.
 *
 * External/internal, invisible/visible participants.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $meeting_id
 *   Meeting ID.
 */
function _os2dagsorden_create_agenda_render_drag_and_drop_controls(&$form, $meeting_id) {
  $form[] = array(
    '#markup' => "<div class=\"select-participants\">",
  );

  $form['internal_participants_div'] = array(
    '#type' => 'item',
    '#prefix' => '<label class="label-participants">' . t('Internal participants:') . '</label><div class="note">' . t('Invisible:') . '</div>',
    '#markup' => '',
  );
  $form['internal_participants_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'internal_participants_hidden',
    ),
  );
  $form['external_participants_div'] = array(
    '#type' => 'item',
    '#prefix' => '<label class="label-participants">' . t('External participants:') . '</label><div class="note">' . t('Invisible:') . '</div>',
    '#markup' => '',
  );
  $form['external_participants_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'external_participants_hidden',
    ),
  );
  $form['participants_invisible_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'participants_invisible_hidden',
    ),
  );
  $form[] = array(
    '#markup' => '</div >',
  );
  $external_participants = array();
  $internal_participants = array();
  $invisible_participants = array();
  if ($meeting_id) {
    $meeting = node_load($meeting_id);
    // external_participants.
    if ($external_participants_items = field_get_items('node', $meeting, 'field_os2web_meetings_partic_ext')) {
      foreach ($external_participants_items as $external_participant) {
        $external_participants[] = $external_participant['uid'];
      }
    }
    if ($internal_participants_items = field_get_items('node', $meeting, 'field_os2web_meetings_partic_int')) {
      foreach ($internal_participants_items as $internal_participant) {
        $internal_participants[] = $internal_participant['uid'];
      }
    }
    if ($invisible_participants_items = field_get_items('node', $meeting, 'field_os2web_meetings_partic_inv')) {
      foreach ($invisible_participants_items as $invisible_participant) {
        $invisible_participants[] = $invisible_participant['uid'];
      }
    }
  }
  // Printing follows plus plus.
  $content = "<ul id='internal_participants' class='droptrue ui-sortable'>";
  foreach ($internal_participants as $uid) {
    $user = user_load($uid);
    $user_name = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
    $form['internal_participants_hidden']['#value'] .= $uid . ',';
    $content .= "<li id='{$uid}' class='ui-state-default'><span>{$user_name}</span>";
    $checked = '';
    if (in_array($uid, $invisible_participants)) {
      $checked = "checked='checked'";
    }
    $content .= "<input id='checkbox_{$uid}'  type='checkbox' class='checkbox invisible' {$checked}></li>";
  }
  $content .= '</ul>';
  $form['internal_participants_div']['#markup'] = $content;
  // Printing follows plus.
  $content = "<ul id='external_participants' class='droptrue ui-sortable'>";
  foreach ($external_participants as $uid) {
    $user = user_load($uid);
    $user_name = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
    $form['external_participants_hidden']['#value'] .= $uid . ',';
    $content .= "<li id='{$uid}' class='ui-state-default'><span>{$user_name}</span>";
    $checked = '';
    if (in_array($uid, $invisible_participants)) {
      $checked = "checked='checked'";
    }
    $content .= "<input id='checkbox_{$uid}' type='checkbox' class='checkbox invisible' {$checked}></li>";
  }
  $content .= '</ul>';
  $form['external_participants_div']['#markup'] = $content;

  $form['participants_invisible_hidden']['#value'] = implode(',', $invisible_participants);

  // Printing arrows reverse.
  $form[]['#markup'] =
    '<div class="arrows-reverse">
    <img src = "' . base_path() . drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/img_arrows_reverse.png">
    </div>';

  $user_list = "<ul id='available_users' class='droptrue ui-sortable'>";
  $users = entity_load('user');
  foreach ($users as $user) {
    if ((int) $user->uid === 0) {
      continue;
    }
    if (in_array($user->uid, $internal_participants) || in_array($user->uid, $external_participants)) {
      continue;
    }
    $user_name = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
    $user_list .= "<li id='{$user->uid}' class='ui-state-default'><span>{$user_name}</span><input id='checkbox_$user->uid'  type='checkbox' class='checkbox'></li>";
  }
  $user_list .= "</ul>";
  $form['available_users'] = array(
    '#type' => 'item',
    '#prefix' => "<div class='available_users'><label>" . t('Users:') . "</label>"
    . "<div class='user-filter-block'><input id='users_filter' type='text' class='form-text'></div>",
    '#markup' => $user_list,
    '#suffix' => '</div>',
  );
  $form[] = array(
    '#markup' => '<div style="clear:both"></div>',
  );
}
