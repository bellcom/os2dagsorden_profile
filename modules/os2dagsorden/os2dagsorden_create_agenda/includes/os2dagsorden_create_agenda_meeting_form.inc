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
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_DAGSORDEN_PLUS => t('Dagsorden+'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_REFERAT_PLUS => t('Referat+'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PREMEETING => t('Formøde'),
      OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PREMEETING_PLUS => t('Formøde+')
    ),
    '#multiple' => FALSE,
    '#default_value' => OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PLANNED,
    '#ajax' => array(
      'callback' => 'os2dagsorden_create_agenda_populate_available_users',
      'event' => 'change',
      'wrapper' => 'available_users',
      'method' => 'replace',
    )
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
  if (isset($form_state['input']['start_date']['date'])) {
    $form['end_date']['#value']['date'] = $form_state['input']['start_date']['date'];
  }
  if (isset($form_state['input']['start_date']['time'])) {
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
    '#ajax' => array(
      'callback' => 'os2dagsorden_create_agenda_populate_available_users',
      'event' => 'change',
      'wrapper' => 'available_users',
      'method' => 'replace',
    )
  );

  $form['field_os2web_meetings_full_attac'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show attachments in full view'),

  );
  if(variable_get('os2dagsorden_show_bp_title_system_text', TRUE)) {
    $form['field_os2web_meetings_punkt_num'] = array(
      '#type' => 'checkbox',
      '#title' => t('Vis Punkt nummerering'),
      '#default_value' => TRUE,
    );
  }

  $form['send_meeting_invitation'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send møder indkaldelse'),
    '#default_value' => FALSE,
  );


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

  $date_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_date'));

  $date_start_obj = date_create($date_field['value'], new DateTimeZone('UTC'));
  date_timezone_set($date_start_obj, timezone_open(date_default_timezone()));
  $form['start_date']['#default_value'] = date_format($date_start_obj, 'Y-m-d H:i:s');

  $date_end_obj = date_create($date_field['value2'], new DateTimeZone('UTC'));
  date_timezone_set($date_end_obj, timezone_open(date_default_timezone()));
  $form['end_date']['#default_value'] = date_format($date_end_obj, 'Y-m-d H:i:s');


  $committee_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_committee'));
  $form['os2web_meetings_tax_committee']['#default_value'] = $committee_field['tid'];

  if (field_get_items('node', $node, 'field_os2web_meetings_location')) {
    $location_field = array_pop(field_get_items('node', $node, 'field_os2web_meetings_location'));
    $form['location']['#default_value'] = $location_field['tid'];
  }
  if ($punkt_numbering = field_get_items('node', $node, 'field_os2web_meetings_punkt_num')) {
    $punkt_numbering = array_pop($punkt_numbering);
    $form['field_os2web_meetings_punkt_num']['#default_value'] = $punkt_numbering['value'];
  }

  if ($attachment_view = field_get_items('node', $node, 'field_os2web_meetings_full_attac')) {
    $attachment_view = array_pop($attachment_view);
    $form['field_os2web_meetings_full_attac']['#default_value'] = $attachment_view['value'];
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
 * Callback for populate available users list
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The current state of the form.
 */
function os2dagsorden_create_agenda_populate_available_users($form, &$form_state) {
  $internal_participants = explode(',', $form_state['values']['internal_participants_hidden']);
  $external_participants = explode(',', $form_state['values']['external_participants_hidden']);
  $meeting_type = $form_state['values']['status'];
  $committee_tid =  $form_state['values']['os2web_meetings_tax_committee'];
  $user_list = "<ul id='available_users' class='droptrue ui-sortable'>";
  $users = entity_load('user');
  foreach ($users as $user) {
    if ((int) $user->uid === 0) {
      continue;
    }
    if (in_array($user->uid, $internal_participants) || in_array($user->uid, $external_participants)) {
      continue;
    }

    if (os2dagsorden_create_agenda_check_user_can_see_meetings_in_committee( $user->uid, $committee_tid, $meeting_type) === FALSE) {
      continue;
    }

    $user_name = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
    $user_list .= "<li id='{$user->uid}' class='ui-state-default'><span>{$user_name}</span><input id='checkbox_$user->uid'  type='checkbox' class='checkbox'></li>";
  }
  $user_list .='</ul>';
  return $user_list;
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
  $values = $form_state['values'];
  // Input extraction.
  $title = isset($values['title'])?$values['title'] : $input['title'];
  $status = $input['status'];
  $start_date = $input['start_date']['date'];
  $start_time = $input['start_date']['time'];
  $end_date = $input['end_date']['date'];
  $end_time = $input['end_date']['time'];
  $loc_tid = $input['location'];
  $com_tid = $input['os2web_meetings_tax_committee'];
  $participants = $input['participants'];
  $referents = $input['referents'];
  $attachment_view = $form_state['values']['field_os2web_meetings_full_attac'];

  $participants = explode(',', $input['participants_hidden']);
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
  $start_datetime = date_create_from_format("d/m/Y H:i", $start_date . ' ' . $start_time, new DateTimeZone(date_default_timezone()));
  date_timezone_set($start_datetime, timezone_open('UTC'));
  $node->field_os2web_meetings_date['und'][0]['value'] = $start_datetime->format('Y-m-d H:i:s');
  $node->field_os2web_meetings_committee['und'][0]['tid'] = $com_tid;
  $node->field_os2web_meetings_full_attac['und'][0]['value'] = $attachment_view;
  if (isset($form_state['values']['field_os2web_meetings_punkt_num'])) {
    $node->field_os2web_meetings_punkt_num['und'][0]['value'] = $form_state['values']['field_os2web_meetings_punkt_num'];
  }



  if (!empty($end_date)) {
    $end_datetime = date_create_from_format("d/m/Y H:i", $end_date . ' ' . $end_time, new DateTimeZone(date_default_timezone()));
    date_timezone_set($end_datetime, timezone_open('UTC'));
    $node->field_os2web_meetings_date['und'][0]['value2'] = $end_datetime->format('Y-m-d H:i:s');
    // Same as start date - default.
  }
  else {
    $node->field_os2web_meetings_date['und'][0]['value2'] = $start_datetime->format('Y-m-d H:i:s');
  }

  $node->field_os2web_meetings_location['und'][0]['tid'] = $loc_tid;

  // Unsetting internal members.
  $node->field_os2web_meetings_partic_int['und'] = array();
  $node->field_os2web_meetings_partic_ext['und'] = array();
  if ($participants) {
    foreach ($participants as $uid) {
      if (!empty($uid)) {
        $participant =  user_load($uid);
        if (intval($participant->field_user_external['und'][0]['value']) == 1) {
          $node->field_os2web_meetings_partic_ext['und'][]['uid'] = $uid;
        }
        else {
          $node->field_os2web_meetings_partic_int['und'][]['uid'] = $uid;
        }
      }
    }
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
 * Fills forms with drag and drop controls for manipulating participants.
 *
 * External/internal, invisible/visible participants.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $meeting_id
 *   Meeting ID.
 */
function _os2dagsorden_create_agenda_render_drag_and_drop_controls(&$form, $form_state, $meeting_id) {
  $form[] = array(
    '#markup' => "<div class=\"select-participants\">",
  );

  $form['participants_div'] = array(
    '#type' => 'item',
    '#prefix' => '<label class="label-participants">' . t('Participants:') . '</label><div class="note">' . t('Invisible:') . '</div><div class="note external-user">' . t('External user') . '</div>',
    '#markup' => '',
  );
  $form['participants_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'participants_hidden',
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
  $participants = array();
  $invisible_participants = array();
  $meeting_type = NULL;
  $committee_tid = NULL;
  if ($meeting_id) {
    $meeting = node_load($meeting_id);
    // external_participants.
    if ($external_participants_items = field_get_items('node', $meeting, 'field_os2web_meetings_partic_ext')) {
      foreach ($external_participants_items as $external_participant) {
        $participants[] = $external_participant['uid'];
      }
    }
    if ($internal_participants_items = field_get_items('node', $meeting, 'field_os2web_meetings_partic_int')) {
      foreach ($internal_participants_items as $internal_participant) {
        $participants[] = $internal_participant['uid'];
      }
    }
    if ($invisible_participants_items = field_get_items('node', $meeting, 'field_os2web_meetings_partic_inv')) {
      foreach ($invisible_participants_items as $invisible_participant) {
        $invisible_participants[] = $invisible_participant['uid'];
      }
    }
    if ($meeting_type = field_get_items('node', $meeting, 'field_os2web_meetings_type')) {
      $meeting_type = $meeting_type[0]['value'];
    }

    if($committee = field_get_items('node', $meeting, 'field_os2web_meetings_committee')) {
      $committee_tid = $committee[0]['tid'];
    }

  }
  if (isset($form_state['input']['participants_hidden']) && !empty($form_state['input']['participants_hidden'])) {
    $participants = $participants + explode(',', $form_state['input']['participants_hidden']);
    $participants = array_unique($participants);

  }
  // Printing follows plus plus.
  $content = "<ul id='participants' class='droptrue ui-sortable'>";
  foreach ($participants as $uid) {
    $user = user_load($uid);
    $user_name = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
    $form['participants_hidden']['#value'] .= $uid . ',';
    if (intval($user->field_user_external['und'][0]['value']) == 1) {
      $content .= "<li id='{$uid}' class='ui-state-default'><span>{$user_name}</span><span class='checkbox external-user'></span>";
    }
    else {
      $content .= "<li id='{$uid}' class='ui-state-default'><span>{$user_name}</span>";
    }
    $checked = '';
    if (in_array($uid, $invisible_participants)) {
      $checked = "checked='checked'";
    }
    $content .= "<input id='checkbox_{$uid}'  type='checkbox' class='checkbox invisible' {$checked}></li>";
  }
  $content .= '</ul>';
  $form['participants_div']['#markup'] = $content;
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
    if (in_array($user->uid, $participants)) {
      continue;
    }

    if (os2dagsorden_create_agenda_check_user_can_see_meetings_in_committee($user->uid, $committee_tid, $meeting_type) === FALSE) {
      continue;
    }
    $user_name = $user->field_user_full_name['und'][0]['value'] . " ($user->name)";
    $external_user = '';
    if (intval($user->field_user_external['und'][0]['value']) == 1) {
      $external_user = '<span class="checkbox external-user"></span>';
    }
    $user_list .= "<li id='{$user->uid}' class='ui-state-default'><span>{$user_name}</span>" . $external_user . "<input id='checkbox_$user->uid'  type='checkbox' class='checkbox'></li>";
  }
  $user_list .= "</ul>";
  $form['available_users'] = array(
    '#type' => 'item',
    '#prefix' => "<div class='available_users'><label>" . t('Users:') . "</label>"
    . "<div class='user-filter-block'><input id='users_filter' type='text' class='form-text'></div>"
    . "<div class='note external-user'>" . t('External user') . "</div>",
    '#markup' => $user_list,
    '#suffix' => '</div>',
  );
  $form[] = array(
    '#markup' => '<div style="clear:both"></div>',
  );
}


/**
 * Checks is given meetings in the committees can bee seen by the user.
 *
 * The function checks user role, if user has role 'os2dagsorden limited' then
 * only his own committees would be visible for him.
 *
 * @param int $meeting
 *   The ID of the meeting or meeting object.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_create_agenda_check_user_can_see_meetings_in_committee($uid, $committee_tid = NULL, $meeting_type = NULL) {
  $user = user_load($uid);

  if (!isset($committee_tid)) {
    return TRUE;
  }
  $committee = taxonomy_term_load($committee_tid);
  // Checking category.
  if (isset($committee->field_os2web_meetings_com_categ['und'])) {
    $meeting_category = $committee->field_os2web_meetings_com_categ['und'][0]['tid'];
    if (isset($user->field_user_com_forbid_categ['und'])) {
      foreach ($user->field_user_com_forbid_categ['und'] as $user_cat) {
        if ($user_cat['tid'] == $meeting_category) {
          return FALSE;
        }
      }
    }
  }

  // Has os2dagsorden limited role, perform committees check.
  if (in_array('os2dagsorden limited', $user->roles)) {
    // Is part of the committee.
    if (in_array($committee_tid, os2dagsorden_access_helper_get_user_member_of_committees($uid))) {
      return TRUE;
    }

    // Follow PP.
    if (in_array($committee_tid, os2dagsorden_access_helper_get_user_follow_pp_committees($uid))) {
      return TRUE;
    }

    // None of the above.
    return FALSE;
  }

    // Checking if is meeting in hidden agendas list.
  if (isset($meeting_type)){
    $hidden_agendas = explode(',', variable_get('os2dagsorden_hidden_agenda_committee'));
    if (strpos($meeting_type, OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_DAGSORDEN) === 0) {
      if (isset($committee->field_os2web_meetings_com_subid['und'])) {
        foreach ($committee->field_os2web_meetings_com_subid['und'] as $id_raw) {
          if (in_array(intval($id_raw['value']), $hidden_agendas)) {
            $in_hidden_list = TRUE;
            break;
          }
        }
      }

      if ($in_hidden_list) {
        // Is part of the committee.
        if (in_array($committee_tid, os2dagsorden_access_helper_get_user_member_of_committees())) {
          return TRUE;
        }

        // Follow PP.
        if (in_array($committee_tid, os2dagsorden_access_helper_get_user_follow_pp_committees())) {
          return TRUE;
        }

        // Follow Plus.
        if (in_array($committee_tid, os2dagsorden_access_helper_get_user_follow_plus_committees())) {
          return TRUE;
        }

        // None of the above, access denied.
        return FALSE;
      }
    }
  }

  // No restrictions, allow access.
  return TRUE;
}
