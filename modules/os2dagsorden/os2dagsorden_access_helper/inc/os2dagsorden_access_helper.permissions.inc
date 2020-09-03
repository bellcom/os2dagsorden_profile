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

//external user
  if (is_object($meeting)) {
    if ((int) $user->field_user_external['und'][0]['value'] === 1) {
      if (isset($meeting->field_os2web_meetings_partic_ext['und'])) {
        foreach ($meeting->field_os2web_meetings_partic_ext['und'] as $ext_partic) {
          if ($ext_partic['uid'] == $user->uid) {
            return TRUE;
          }
        }
      }
      return FALSE;
    }
  }

  $committee = taxonomy_term_load($committee_tid);
  $is_external_committee = array_pop(field_get_items('taxonomy_term', $committee, 'field_os2web_meetings_com_ext'));
  $is_external_committee = $is_external_committee['value'];

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
//

// Checking if is meeting in external committee
    if ($is_external_committee && variable_get('os2dagsorden_meetings_views_last_8_show_external_committee', TRUE)) {
      if (in_array($committee_tid, os2dagsorden_access_helper_get_user_member_of_committees($uid))) {
        return TRUE;
      }
      else {
        return FALSE;
      }
    }
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
// Is in white-list
        $users_white_list = drupal_explode_tags(variable_get('os2dagsorden_users_accessed_hidden_agenda_committee'));
        $users_white_list = array_map('trim', $users_white_list);

        if (in_array($user->name, $users_white_list)) {
          return TRUE;
        }

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
    $publish_level = $bullet_points[$bullet_point_id]->field_os2web_meetings_bul_plevel['und'][0]['value'];
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
      if (os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id)) {
        return TRUE;
      }
      else if (variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE)) {
        $bullet_point = node_load($bullet_point_id);
        $publish_level = $bullet_point->field_os2web_meetings_bul_plevel['und'][0]['value'];
        return os2dagsorden_access_helper_bullet_point_attachment_is_public($publish_level, $bullet_point_attachment_id);
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
      if (variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE)) {
        return os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment_content($meeting_id, $bullet_point_id, $bullet_point_attachments_id);
      }
      else {
        return os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);

      }
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
  if (isset($committee->field_os2web_meetings_com_ext['und']) && (int) $committee->field_os2web_meetings_com_ext['und'][0]['value'] === 1) {
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
 * Check if bullet point has content which can be shown for all users.
 *
 * @param int $bullet_point_id
 *   Bullet point nid.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_check_bullet_point_has_public_content($bullet_point_id) {
  $bullet_point = node_load($bullet_point_id);
  $is_closed = $bullet_point->field_os2web_meetings_bul_closed['und'][0]['value'];
  $is_personal = $bullet_point->field_os2web_meetings_bul_pers['und'][0]['value'];
  $publish_level = $bullet_point->field_os2web_meetings_bul_plevel['und'][0]['value'];
  // Is opened and not personal.
  if (intval($is_closed) == 0 && intval($is_personal) == 0) {
    return TRUE;
  }
  if (intval($is_personal) == 1) {
    return FALSE;
  }
  // Check if Bullet Point has open BPAs.
  if (variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE)) {
    foreach ($bullet_point->field_os2web_meetings_attach['und'] as $ref_bullet_point_attachment) {
      if (os2dagsorden_access_helper_bullet_point_attachment_is_public($publish_level, $ref_bullet_point_attachment['target_id'])) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Check if bullet point attachment (content or title) can be shown for all users.
 * Access rights depends of Bullet Point publish level.
 *
 * @param int $bp_publish_level
 *   Bullet point publish level.
 *
 * @param int $$attachment_id
 *   Bullet point attachment_nid.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_bullet_point_attachment_is_public($bp_publish_level, $attachment_id) {
  $attachment = node_load($attachment_id);

  if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
   return FALSE;
  }
  // It's bilag.
  if (!empty($attachment->field_os2web_meetings_bpa_file['und'])) {
    // Check if BPAs is bilag and it's publish level > 0.
    if (!empty($attachment->field_os2web_meetings_bpa_plevel) && intval($attachment->field_os2web_meetings_bpa_plevel['und'][0]['value']) > 0) {
      return TRUE;
    }
  }
  else {
    // All attahcments can be shown.
    if (intval($bp_publish_level) == OS2DAGSORDEN_ACCESS_HELPER_CLOSED_BP_FULL_ACCESS) {
      return TRUE;
    }
    // Only beslutning.
    else if (intval($bp_publish_level) == OS2DAGSORDEN_ACCESS_HELPER_CLOSED_BP_TITLE_DESICION && strpos(strtolower($attachment->title), 'beslutning') === 0) {
      return TRUE;
    }
    // Only beslutning and indstilling.
    else if (intval($bp_publish_level) == OS2DAGSORDEN_ACCESS_HELPER_CLOSED_BP_TITLE_SUGGESTED_DESICION &&
        (strpos(strtolower($attachment->title), 'beslutning') === 0 || strpos(strtolower($attachment->title), 'indstilling') === 0)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Checks if current user is allowed to see content of bullet point attachment.
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
function os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment_content($meeting_id, $bullet_point_id, $bullet_point_attachment_id) {
  $user_ent = os2dagsorden_access_helper_get_user();
  if (!$user_ent) {
    return FALSE;
  }
  $attachment = node_load($bullet_point_attachment_id);
  $bullet_point = node_load($bullet_point_id);

  // If user has fully access to Bullet point.
  if (os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id)) {
    return TRUE;
  }
  $publish_level = $bullet_point->field_os2web_meetings_bul_plevel['und'][0]['value'];
  if (!empty($attachment->field_os2web_meetings_bpa_file['und'])) {

    if (os2dagsorden_access_helper_bullet_point_attachment_is_public($publish_level, $bullet_point_attachment_id)) {
      if (empty($attachment->field_os2web_meetings_bpa_plevel)) {
        return TRUE;
      }
      else {
        if (intval($attachment->field_os2web_meetings_bpa_plevel['und'][0]['value']) == 2) {
          return TRUE;
        }
      }
    }
  }
  else {
    return os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bullet_point_id, $bullet_point_attachment_id);
  }
  return FALSE;
}

/**
 * Checks if user has access to bullet point page.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 *
 * @return bool
 *   True of false.
 */

function os2dagsorden_access_helper_check_user_has_access_to_bullet_point($meeting_id, $bullet_point_id) {
  $user_ent = os2dagsorden_access_helper_get_user();
  if (!$user_ent) {
    return FALSE;
  }
  $is_accessible = (os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id) || os2dagsorden_access_helper_check_bullet_point_has_public_content($bullet_point_id));
  return $is_accessible;
}

/**
 * Return Bullet point title.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param string $title
 *    Bullet point title.
 * @param int $publish_level
 *    Bullet point publish level.
 *
 * @return string
 *   Title
 */
function os2dagsorden_access_helper_get_bilag_title($meeting_id, $bullet_point_id, $title, $publish_level) {
  $user_can_see_bullet_point =  os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
  if (!$user_can_see_bullet_point && $publish_level === OS2DAGSORDEN_ACCESS_HELPER_CLOSED_BPA_ANONYMOUS_TITLE) {
    $title = variable_get('os2dagsorden_title_anonymiseret', 'Anonymiseret');
  }
return $title;
}

/**
 * Return bilag title.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param string $title
 *    Bilag title.
 * @param int $publish_level
 *    Bilag publish level.
 *
 * @return string
 *   Title.
 */
function os2dagsorden_access_helper_get_bullet_point_title($meeting_id, $bullet_point_id, $title, $publish_level) {
  $user_can_see_bullet_point =  os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
  if (!$user_can_see_bullet_point && $publish_level === OS2DAGSORDEN_ACCESS_HELPER_CLOSED_BP_ANONYMOUS_TITLE) {
    $title = variable_get('os2dagsorden_title_anonymiseret', 'Anonymiseret');
  }
 return $title;
}
