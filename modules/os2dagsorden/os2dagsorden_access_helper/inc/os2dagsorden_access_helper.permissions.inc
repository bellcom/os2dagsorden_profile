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
 * Checks is given meeting can bee seen by the user. The function checks user role, if user has role 'os2dagsorden limited' then only his own committees would be visible for him.
 *
 * @param int $meeting
 *   The ID of the meeting or meeting object.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_user_can_see_meeting($meeting, $committee_tid = NULL) {
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
  $committee = taxonomy_term_load($committee_tid);

  // Checking category.
  if (isset($committee->field_os2web_meetings_com_cat['und'])) {
    $meeting_category = $committee->field_os2web_meetings_com_cat['und'][0]['value'];
    if (isset($user->field_user_forrbidden_categories['und'])) {
      foreach ($user->field_user_forrbidden_categories['und'] as $user_cat) {
        if ($user_cat['value'] == $meeting_category) {
          return FALSE;
        }
      }
    }
  }

  // Has os2dagsorden limited role, perform committees check.
  if (in_array('os2dagsorden limited', $user->roles)) {
    // Is part of the committee.
    if (in_array($committee_tid, os2dagsorden_access_helper_get_user_member_of_committees())) {
      return TRUE;
    }

    // Follow PP.
    if (in_array($committee_tid, os2dagsorden_access_helper_get_user_follow_pp_committees())) {
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
    if (strpos($meeting_type, 'Premeeting') === 0) {
      if (!in_array($committee_tid, os2dagsorden_access_helper_get_user_premeeting_member_of_committees())) {
        return FALSE;
      }
    }

    // Checking if is meeting in hidden agendas list.
    $hidden_agendas = explode(',', variable_get('os2dagsorden_hidden_agenda_committee'));
    if (strpos(strtolower($meeting->field_os2web_meetings_type['und'][0]['value']), "dagsorden") !== FALSE) {
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

  if (isset($committee->field_os2web_meetings_com_cat['und'])) {
    $meeting_category = $committee->field_os2web_meetings_com_cat['und'][0]['value'];
    if (isset($user->field_user_meetings_category['und'])) {
      foreach ($user->field_user_meetings_category['und'] as $user_cat) {
        if ($user_cat['value'] == $meeting_category) {
          $access_granted = TRUE;
          break;
        }
      }
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
function os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id) {
  $user_ent = os2dagsorden_access_helper_get_user();
  global $user;

  if (!$user_ent->uid) {
    return FALSE;
  }

  $meeting = node_load($meeting_id);
  if (!node_access('view', $meeting)) {
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
    // Is closed or personal.
    else {
      $meeting_committee_id = $meeting->field_os2web_meetings_committee['und'][0]['tid'];

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

      // Is closed and not personal.
      if (intval($is_closed) == 1 && intval($is_personal) == 0) {
        // Follows committee PLUS.
        if (in_array($meeting_committee_id, os2dagsorden_access_helper_get_user_follow_plus_committees())) {
          return TRUE;
        }
      }
      // None of the above, access denied.
      return FALSE;
    }
  }
  else {
    return FALSE;
  }
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
      return os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
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
