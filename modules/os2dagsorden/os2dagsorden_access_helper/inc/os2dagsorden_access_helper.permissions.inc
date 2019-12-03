<?php

/**
 * @file
 * Os2dagsorden_access_helper.permissions.inc.
 *
 * Various permissions and access related functions.
 *
 * @category OS2Dagsorden
 * @package OS2Dagsorden_Access_Helper
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/licenses/gpl-2.0.html GNU General Public License, version 2
 * @link http://bellcom.dk
 */

/**
 * Checks is given meeting can bee seen by the user.
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
function os2dagsorden_access_helper_check_user_can_see_meeting($meeting, $committee_tid = NULL, $uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  if (!$user->uid) {
    return FALSE;
  }
  if (!is_object($meeting)) {
    $meeting = node_load($meeting);
  }

  if (!$committee_tid) {
    $committee_tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
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

  if (is_object($meeting)) {
    $meeting_type = $meeting->field_os2web_meetings_type['und'][0]['value'];
    // Is meeting unpublished?
    if ($meeting->status == 0 && !_os2dagsorden_access_helper_check_user_is_committee_admin($meeting_type, $committee_tid, $user->uid)) {
      return FALSE;
    }

    // Forbidding access to premeeting.
    // if (strpos($meeting_type, OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PREMEETING) === 0) {
    // if (!in_array($committee_tid, os2dagsorden_access_helper_get_user_premeeting_member_of_committees())) {
    // return FALSE;
    // }
    // }

    // Checking if is meeting in hidden agendas list.
    $hidden_agendas = explode(',', variable_get('os2dagsorden_hidden_agenda_committee'));
    if (strpos($meeting->field_os2web_meetings_type['und'][0]['value'], OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_DAGSORDEN) === 0) {
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
        if (in_array($committee_tid, os2dagsorden_access_helper_get_user_member_of_committees($uid))) {
          return TRUE;
        }

        // Follow PP.
        if (in_array($committee_tid, os2dagsorden_access_helper_get_user_follow_pp_committees($uid))) {
          return TRUE;
        }

        // Follow Plus.
        if (in_array($committee_tid, os2dagsorden_access_helper_get_user_follow_plus_committees($uid))) {
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

/**
 * Checks is given meeting can bee seen by the user on calendar page.
 *
 * @param int $meeting
 *   The ID of the meeting or meeting object.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_meeting_in_calendar($meeting, $committee_tid = NULL) {
  $user = os2dagsorden_access_helper_get_user();
  if (!$user->uid) {
    return FALSE;
  }
  if (!is_object($meeting)) {
    $meeting = node_load($meeting);
  }

  if (!$committee_tid) {
    $committee_tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
  }

  // Checking category.
  $committee = taxonomy_term_load($committee_tid);
  $access_granted = FALSE;

  if (isset($committee->field_os2web_meetings_com_categ['und'])) {
    $meeting_category = $committee->field_os2web_meetings_com_categ['und'][0]['tid'];
    if (isset($user->field_user_com_categ['und'])) {
      foreach ($user->field_user_com_categ['und'] as $user_cat) {
        if ($user_cat['tid'] == $meeting_category) {
          $access_granted = TRUE;
          break;
        }
      }
    }
    else {
      $access_granted = TRUE;
    }

  }
  return $access_granted;
}

/**
 * Checks if a user can see the closed agenda of a given meeting.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_closed_agenda($meeting_id) {
  $user_ent = os2dagsorden_access_helper_get_user();
  if (!$user_ent->uid) {
    return FALSE;
  }

  $meetings = entity_load('node', array($meeting_id), array('type' => 'os2web_meetings_meeting'));
  $meeting_committee_id = $meetings[$meeting_id]->field_os2web_meetings_committee['und'][0]['tid'];
  if (!os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meetings[$meeting_id])) {
    return FALSE;
  }

  // Is part of premeeting committee.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_premeeting_member_of_committees())) {
    return TRUE;
  }

  // Is part of the committee.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_member_of_committees())) {
    return TRUE;
  }

  // Follows PLUS.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_plus_committees())) {
    return TRUE;
  }

  // Follow PP.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_pp_committees())) {
    return TRUE;
  }

  // None of the above, access denied.
  return FALSE;
}

/**
 * Checks if a user can see the personal agenda of a given meeting.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_personal_agenda($meeting_id) {
  $user_ent = os2dagsorden_access_helper_get_user();
  if (!$user_ent->uid) {
    return FALSE;
  }
  $meetings = entity_load('node', array($meeting_id), array('type' => 'os2web_meetings_meeting'));
  $meeting_committee_id = $meetings[$meeting_id]->field_os2web_meetings_committee['und'][0]['tid'];
  if (!os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meetings[$meeting_id])) {
    return FALSE;
  }
  // Is part of premeeting committee.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_premeeting_member_of_committees())) {
    return TRUE;
  }

  // Is part of the committee.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_member_of_committees())) {
    return TRUE;
  }

  // Follow PP.
  if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_pp_committees())) {
    return TRUE;
  }

  // None of the above, access denied.
  return FALSE;
}

/**
 * Checks if current user is allowed the access the bullet point.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id, $uid = NULL) {
  if (!$uid) {
    $user_ent = os2dagsorden_access_helper_get_user();
  }
  else {
    $user_ent = user_load($uid);
  }

  if (!$user_ent->uid) {
    return FALSE;
  }

  $meeting = node_load($meeting_id);
  $meeting_committee_id = $meeting->field_os2web_meetings_committee['und'][0]['tid'];

  if (!os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting, $uid)) {
    return FALSE;
  }
  elseif (!node_access('view', $meeting)) {
    return FALSE;
  }

  if (os2dagsorden_access_helper_check_bullet_point_is_part_of_meeting($meeting_id, $bullet_point_id)) {

    $bullet_points = entity_load('node', array($bullet_point_id), array('type' => 'os2web_meetings_bullet'));
    $is_closed = $bullet_points[$bullet_point_id]->field_os2web_meetings_bul_closed['und'][0]['value'];
    $is_personal = $bullet_points[$bullet_point_id]->field_os2web_meetings_bul_pers['und'][0]['value'];

    // Is opened and not personal.
    if (intval($is_closed) == 0 && intval($is_personal) == 0) {
      return TRUE;
    }
    // Is closed.
    if ((intval($is_closed) == 1)) {
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_member_of_committees($uid))) {
        return TRUE;
      }
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_pp_committees($uid))) {
        return TRUE;
      }
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_plus_committees($uid))) {
        return TRUE;
      }
      if (variable_get('os2dagsorden_open_specified_bpa_in_closed_bullet')) {
        $accesible_bpa_titles = explode(',', variable_get('os2dagsorden_fully_accesibe_bpas_title_list', ""));
        if (is_array($accesible_bpa_titles) && !empty($accesible_bpa_titles)) {
          return _os2dagsorden_access_helper_check_closed_bullet_point_has_fully_accesible_bpa($bullet_points[$bullet_point_id], $accesible_bpa_titles);
        }
      }
    }
    // Is personal.
    if ((intval($is_personal) == 1)) {
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_member_of_committees($uid))) {
        return TRUE;
      }
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_pp_committees($uid))) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Check if the user has permission to share a specific bullet point.
 *
 * @param int $meeting_id
 *   Meeting nid.
 * @param int $bullet_point_id
 *   Bullet point nid.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_check_user_can_send_bullet_point($meeting_id, $bullet_point_id) {
  $user_ent = os2dagsorden_access_helper_get_user();

  if (!$user_ent->uid) {
    return FALSE;
  }

  $meeting = node_load($meeting_id);
  if (!node_access('view', $meeting)) {
    return FALSE;
  }
  if (!os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting)) {
    return FALSE;
  }

  if (os2dagsorden_access_helper_check_bullet_point_is_part_of_meeting($meeting_id, $bullet_point_id)) {
    $bullet_points = entity_load('node', array($bullet_point_id), array('type' => 'os2web_meetings_bullet'));
    $is_closed = $bullet_points[$bullet_point_id]->field_os2web_meetings_bul_closed['und'][0]['value'];
    $is_personal = $bullet_points[$bullet_point_id]->field_os2web_meetings_bul_pers['und'][0]['value'];

    // Is opened and not personal.
    if (intval($is_closed) == 0 && intval($is_personal) == 0) {
      return TRUE;
    }
    else {
      if (intval($user_ent->field_user_can_send_closed_bp['und'][0]['value']) == 1) {
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
  }
}

/**
 * Checks if current user is allowed the access the bullet point attachment.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param int $bullet_point_attachment_id
 *   ID of the attachment.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bullet_point_id, $bullet_point_attachment_id) {
  if (os2dagsorden_access_helper_check_attachment_is_part_of_bullet_point($bullet_point_id, $bullet_point_attachment_id)) {
    $bpa = node_load($bullet_point_attachment_id);
    if ($bpa->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
      $meeting = node_load($meeting_id);
      $meeting_committee_id = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
      if (!os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting)) {
        return FALSE;
      }
      // Is part of premeeting committee.
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_premeeting_member_of_committees())) {
        return TRUE;
      }

      // Is part of the committee.
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_member_of_committees())) {
        return TRUE;
      }

      // Follows PP.
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_pp_committees())) {
        return TRUE;
      }

      // Follows committee PLUS.
      if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_plus_committees())) {
        return TRUE;
      }

      return FALSE;
    }
    else {
      $bullet_point = node_load($bullet_point_id);
      $is_closed = $bullet_point->field_os2web_meetings_bul_closed['und'][0]['value'];
      $user_can_see_bullet_point = os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
      if ($is_closed && $user_can_see_bullet_point && variable_get('os2dagsorden_open_specified_bpa_in_closed_bullet')) {
        $meeting = node_load($meeting_id);
        $meeting_committee_id = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
        if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_member_of_committees($uid))) {
          return TRUE;
        }
        if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_pp_committees($uid))) {
          return TRUE;
        }
        if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_plus_committees($uid))) {
          return TRUE;
        }
        if (_os2dagsorden_access_helper_check_bullet_point_fully_accesible($bpa->title)) {
          return TRUE;
        }
      }
      else {
        return $user_can_see_bullet_point;
      }
    }
  }
  else {
    return FALSE;
  }
}

/**
 * Check if user can download attachment file.
 *
 * @param int $fid
 *   Fid of the file.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_check_user_can_download_bullet_point_attachment_file($fid) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->fieldCondition('field_os2web_meetings_bpa_file', 'fid', $fid, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $bullet_point_attachments_id = reset($nids);
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->fieldCondition('field_os2web_meetings_attach', 'target_id', $bullet_point_attachments_id, '=');
    $result = $query->execute();
    if (isset($result['node'])) {
      $nids = array_keys($result['node']);
      $bullet_point_id = reset($nids);
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'node')
        ->fieldCondition('field_os2web_meetings_bullets', 'target_id', $bullet_point_id, '=');
      $result = $query->execute();
      if (isset($result['node'])) {
        $nids = array_keys($result['node']);
        $meeting_id = reset($nids);
      }
    }
    if (isset($meeting_id) && isset($bullet_point_id)) {
      return os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
    }
  }

  return TRUE;
}

/**
 * Checks is given speaker paper can be seen by user.
 *
 * @param int $sp_id
 *   The ID of speaker paper.
 * @param bool $including_shared
 *   Whether shared speaker papers should be included into decision process.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_speaker_paper($sp_id, $including_shared = FALSE) {
  global $user;
  $sp = node_load($sp_id);

  if (!$including_shared) {
    return ($user->uid == $sp->uid);
  }
  else {
    if ($user->uid == $sp->uid) {
      return TRUE;
    }
    else {
      // Checking sharing with individuals.
      if (isset($sp->field_os2web_meetings_sp_shared['und'])) {
        foreach ($sp->field_os2web_meetings_sp_shared['und'] as $shared_with) {
          if (intval($shared_with['target_id'] == $user->uid)) {
            return TRUE;
          }
        }
      }
      // Checking sharing with groups.
      if (isset($sp->field_os2web_meetings_sp_shar_gr['und'])) {
        foreach ($sp->field_os2web_meetings_sp_shar_gr['und'] as $shared_with_group) {
          return in_array($shared_with_group['tid'], os2dagsorden_access_helper_get_user_sp_groups());
        }
      }
    }
  }

  return FALSE;
}

/**
 * Checks is given meeting i external committee can bee seen by the user.
 *
 * The function checks if user is participants of given meeting then he can see meeting content.
 *
 * @param object $meeting
 *   The meeting object.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_meeting_in_external_committee($meeting, $uid) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }

  if (os2dagsorden_create_agenda_is_user_committee_admin($meeting->field_os2web_meetings_committee['und'][0]['tid'])) {
    return TRUE;
  }

  $participants = array();
  $internal_participants = field_get_items('node', $meeting, 'field_os2web_meetings_partic_int');
  $external_participants = field_get_items('node', $meeting, 'field_os2web_meetings_partic_ext');
  $days_before_meeting_date = (strtotime($meeting->field_os2web_meetings_date['und'][0]['value']) - time()) / 60 / 60 / 24;
  if (is_array($internal_participants)) {
    $participants = array_merge($participants, $internal_participants);
  }
  if (is_array($external_participants)) {
    $participants = array_merge($participants, $external_participants);
  }
  if (!empty($participants)) {
    foreach ($participants as $participant) {
      if ((int) $participant['uid'] === (int) $user->uid) {
        if ($days_before_meeting_date < variable_get('os2dagsorden_timespan_open_meeting_content', 14)) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Checks is given committee is external.
 *
 * @param int $committee_tid
 *   The committee ID.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_is_external_committee($committee_tid) {
  $committee = taxonomy_term_load($committee_tid);
  if (isset($committee->field_os2web_meetings_com_ext['und'])
      && (int) $committee->field_os2web_meetings_com_ext['und'][0]['value'] === 1) {
    return TRUE;
  }
  return FALSE;
}

/**
 * Checks is current user can create meeting in given committee.
 *
 * @param int $committee_tid
 *   The committee ID.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_create_meeting_in_committee($committee_tid) {
  if (os2dagsorden_access_helper_is_external_committee($committee_tid) === TRUE) {
    if (!os2dagsorden_create_agenda_is_user_committee_admin($committee_tid)) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Checks is current user can clear meeting content.
 *
 * @param int $committee_tid
 *   The committee ID.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_delete_meeting_content($committee_tid) {
  if (os2dagsorden_access_helper_is_external_committee($committee_tid) === TRUE) {
    if (os2dagsorden_create_agenda_is_user_committee_admin($committee_tid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Checks is current user can see full meeting content.
 *
 * @param object $meeting
 *   The Meeting.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting, $uid = NULL) {
  $meeting_committee_id = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
  $meeting_type = $meeting->field_os2web_meetings_type['und'][0]['value'];
  if (os2dagsorden_access_helper_is_external_committee($meeting_committee_id) === TRUE) {
    if (!os2dagsorden_access_helper_check_user_can_see_meeting_in_external_committee($meeting, $uid)) {
      return FALSE;
    }
  }
  // Forbidding access to premeeting.
  if (strpos($meeting_type, OS2WEB_MEETINGS_CONTENT_TYPES_MEETING_PREMEETING) === 0) {
    if (!in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_premeeting_member_of_committees($uid))) {
      return FALSE;
    }
  }
  return TRUE;
}


/**
 * Checks if closed bullet point has bullet point attachments that can be accessible for all users.
 *
 * @param mixed $bullet_point
 *   Bullet point.
 * @param mixed $accesible_bpa_titles
 *   List of bullet point attachment titles that should be fully accessible.
 *
 * @return bool
 *   True of false.
 */
function _os2dagsorden_access_helper_check_closed_bullet_point_has_fully_accesible_bpa($bullet_point, $accesible_bpa_titles) {
  $bpa_nids = array();
  foreach($bullet_point->field_os2web_meetings_attach['und'] as $value){
    $bpa_nids[] = $value['target_id'];
  }
  $result = db_select('node', 'n')
    ->fields('n', array('title'))
    ->condition('nid', $bpa_nids,'IN')
    ->execute()
    ->fetchAllAssoc('title');
  if ($result) {
    $titles = array_keys($result);
    $titles = array_map('strtolower', $titles);
    $accesible_bpa_titles = array_map('trim', $accesible_bpa_titles);
    $accesible_bpa_titles = array_map('strtolower', $accesible_bpa_titles);
    return (!empty(array_intersect($titles, $accesible_bpa_titles)));
  }
  return FALSE;
}

/**
 * Checks if closed bullet point attachment is accessible for all users.
 *
 * @param string $title
 *   Bullet point attachment title
 *
 * @return bool
 *   True of false.
 */
function _os2dagsorden_access_helper_check_bullet_point_fully_accesible($title) {
  $accesible_bpa_titles = explode(',', variable_get('os2dagsorden_fully_accesibe_bpas_title_list', ""));
  if (is_array($accesible_bpa_titles) && !empty($accesible_bpa_titles)) {
    $accesible_bpa_titles = array_map('trim', $accesible_bpa_titles);
    $accesible_bpa_titles = array_map('strtolower', $accesible_bpa_titles);
    return in_array(strtolower($title), $accesible_bpa_titles);
  }
  return FALSE;
}
