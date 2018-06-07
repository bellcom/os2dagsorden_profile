<?php

/**
 * @file
 * Os2dagsorden_access_helper.permissions.inc.
 *
 * Various helper functions.
 *
 * @category OS2Dagsorden
 * @package OS2Dagsorden_Access_Helper
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/licenses/gpl-2.0.html GNU General Public License, version 2
 * @link http://bellcom.dk
 */

/**
 * Checks if given meeting has speaker paper of the current user attached.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_meeting_has_speaker_paper($meeting_id) {
  $meeting = node_load($meeting_id);
  if (!os2dagsorden_access_helper_is_user_has_full_access_to_meeting_content($meeting)) {
    return FALSE;
  }
  if (isset($meeting->field_os2web_meetings_bullets['und'])) {
    foreach ($meeting->field_os2web_meetings_bullets['und'] as $bullet_point) {
      if (os2dagsorden_access_helper_bullet_point_has_speaker_paper($bullet_point['target_id'], variable_get('os2dagsorden_shared_speaker_paper', TRUE))) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
 * Checks if given bullet point has speaker paper of the current user attached.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param bool $including_shared
 *   Whether shared speaker papers should be included into decision process.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_bullet_point_has_speaker_paper($bullet_point_id, $including_shared = FALSE) {
  global $user;
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', array('os2web_meetings_spaper'))
    ->propertyCondition('uid', $user->uid)
    ->fieldCondition('field_os2web_meetings_sp_bullet', 'target_id', $bullet_point_id, '=');

  $result = $query->execute();

  if (!isset($result['node']) && $including_shared) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
      ->propertyCondition('type', array('os2web_meetings_spaper'))
      ->fieldCondition('field_os2web_meetings_sp_shared', 'target_id', $user->uid, '=')
      ->fieldCondition('field_os2web_meetings_sp_bullet', 'target_id', $bullet_point_id, '=');

    $result = $query->execute();
  }

  return isset($result['node']);
}

/**
 * Functions returns the user premeeting member of committees.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Committees tids.
 */
function os2dagsorden_access_helper_get_user_premeeting_member_of_committees($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Premeeting committees.
  if (!empty($user->field_user_premeeting_committee)) {
    foreach ($user->field_user_premeeting_committee['und'] as $user_committee_id) {
      $tids[] = $user_committee_id['tid'];
    }
  }

  return $tids;
}

/**
 * Functions returns the user member of committees.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Committees tids.
 */
function os2dagsorden_access_helper_get_user_member_of_committees($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Is part of the committee.
  if (!empty($user->field_user_committee)) {
    foreach ($user->field_user_committee['und'] as $user_committee_id) {
      $tids[] = $user_committee_id['tid'];
    }
  }

  return $tids;
}

/**
 * Functions returns the user follows committees.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Committees tids.
 */
function os2dagsorden_access_helper_get_user_follow_committees($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Is part of the committee.
  if (!empty($user->field_follows_committee)) {
    foreach ($user->field_follows_committee['und'] as $user_committee_id) {
      $tids[] = $user_committee_id['tid'];
    }
  }

  return $tids;
}

/**
 * Functions returns the user follows subscribed committees.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Committees tids.
 */
function os2dagsorden_access_helper_get_user_follow_subscribed_committees($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Is part of the committee.
  if (!empty($user->field_follows_committee_subsc)) {
    foreach ($user->field_follows_committee_subsc['und'] as $user_committee_id) {
      $tids[] = $user_committee_id['tid'];
    }
  }

  return $tids;
}

/**
 * Functions returns the user follows_plus committees.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Committees tids.
 */
function os2dagsorden_access_helper_get_user_follow_plus_committees($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Is part of the committee.
  if (!empty($user->field_follows_committee_plus)) {
    foreach ($user->field_follows_committee_plus['und'] as $user_committee_id) {
      $tids[] = $user_committee_id['tid'];
    }
  }

  return $tids;
}

/**
 * Functions returns the user follows_plus_plus committees.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Committees tids.
 */
function os2dagsorden_access_helper_get_user_follow_pp_committees($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Is part of the committee.
  if (!empty($user->field_follows_committee_pp)) {
    foreach ($user->field_follows_committee_pp['und'] as $user_committee_id) {
      $tids[] = $user_committee_id['tid'];
    }
  }

  return $tids;
}

/**
 * Functions returns the user speaker paper groups.
 *
 * @param int $uid
 *   Uid of the user, if empty, current user is used.
 *
 * @return array
 *   Speaker paper group tids.
 */
function os2dagsorden_access_helper_get_user_sp_groups($uid = NULL) {
  if (!$uid) {
    $user = os2dagsorden_access_helper_get_user();
  }
  else {
    $user = user_load($uid);
  }
  $tids = array();
  // Is part of the committee.
  if (!empty($user->field_user_sp_groups)) {
    foreach ($user->field_user_sp_groups['und'] as $user_group_id) {
      $tids[] = $user_group_id['tid'];
    }
  }

  return $tids;
}

/**
 * Checks if the bullet point is actually part of the meeting.
 *
 * (or any of its additional meetings).
 *
 * @param int $meeting_id
 *   ID of the meeting.
 * @param int $bullet_point_id
 *   ID of the bullet point.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_access_helper_check_bullet_point_is_part_of_meeting($meeting_id, $bullet_point_id) {
  // Creating meeting list to check.
  $possible_meetings = os2dagsorden_access_helper_get_meeting_and_tillaegs_meetings($meeting_id);

  foreach ($possible_meetings as $nid) {
    $meeting = node_load($nid);

    // Checking meetings bullet points.
    if (!empty($meeting->field_os2web_meetings_bullets['und'])) {
      foreach ($meeting->field_os2web_meetings_bullets['und'] as $ref_bullet_point) {
        if (intval($bullet_point_id) == intval($ref_bullet_point['target_id'])) {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

/**
 * Returns an array of meetings additional agendas (tillaegs meetings).
 *
 * @param int $meeting_id
 *   Initial meeting ID.
 *
 * @return array
 *   Array of ids.
 */
function os2dagsorden_access_helper_tillaegs_meetings($meeting_id) {
  $possible_meetings = array();

  // Adding tillaegs meetings, if any.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->fieldCondition('field_os2web_meetings_addendum', 'nid', $meeting_id, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $possible_meetings = array_keys($result['node']);
  }

  return $possible_meetings;
}

/**
 * Returns an array of ids of meeting.
 *
 * And all of its additional agendas (tillaegs meetings).
 *
 * @param int $meeting_id
 *   Initial meeting ID.
 *
 * @return mixed
 *   Array of ids.
 */
function os2dagsorden_access_helper_get_meeting_and_tillaegs_meetings($meeting_id) {
  $possible_meetings = array($meeting_id);

  // Adding tillaegs meetings, if any.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->fieldCondition('field_os2web_meetings_addendum', 'nid', $meeting_id, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $possible_meetings = array_merge($possible_meetings, array_keys($result['node']));
  }

  return $possible_meetings;
}

/**
 * Returns main of (tillaegs meetings).
 *
 * @param int $meeting_id
 *   Tillaegs meeting ID.
 *
 * @return int
 *   Nid of the main meeting.
 */
function os2dagsorden_access_helper_get_main_meeting($meeting_id) {
  $meeting = node_load($meeting_id);

  if (!empty($meeting->field_os2web_meetings_addendum['und'])) {
    return $meeting->field_os2web_meetings_addendum['und'][0]['nid'];
  }
}

/**
 * Returns a meeting associated with this bullet point.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 *
 * @return object
 *   Meeting.
 */
function os2dagsorden_access_helper_get_meeting_by_bullet_point($bullet_point_id) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->fieldCondition('field_os2web_meetings_bullets', 'target_id', $bullet_point_id, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $meeting_id = reset($nids);
    return node_load($meeting_id);
  }
}

/**
 * Returns a bullet point associated with this bullet point attachment.
 *
 * @param int $bpa_id
 *   ID of the bullet point attachment.
 *
 * @return object
 *   Bullet point.
 */
function os2dagsorden_access_helper_get_bullet_point_by_bpa($bpa_id) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->fieldCondition('field_os2web_meetings_attach', 'target_id', $bpa_id, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $bullet_id = reset($nids);
    return node_load($bullet_id);
  }
}

/**
 * Checks if the bullet point attachment is actually part of the bullet point.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param int $bilag_id
 *   ID of the bullet point attachment.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_check_attachment_is_part_of_bullet_point($bullet_point_id, $bilag_id) {
  $bullet_point = node_load($bullet_point_id);

  $contains = FALSE;
  if (!empty($bullet_point->field_os2web_meetings_attach['und'])) {
    foreach ($bullet_point->field_os2web_meetings_attach['und'] as $ref_bullet_point_attachment) {
      if (intval($bilag_id) == intval($ref_bullet_point_attachment['target_id'])) {
        $contains = TRUE;
        break;
      }
    }
  }
  return $contains;
}

/**
 * Checks if all of the meeitng bullet point bilag were converted to pdf/html.
 *
 * And all the meetings descriptions have been converted to pdf.
 *
 * @return bool
 *   TRUE or FALSE.
 */
function os2dagsorden_access_helper_meeting_fully_converted($meeting) {
  // Going through all the meetings bullet points.
  if (!empty($meeting->field_os2web_meetings_bullets['und'])) {
    foreach ($meeting->field_os2web_meetings_bullets['und'] as $ref_bullet_point) {
      $bullet_point = node_load($ref_bullet_point['target_id']);

      // Going through all the bullet points attachments.
      if (!empty($bullet_point->field_os2web_meetings_attach['und'])) {
        foreach ($bullet_point->field_os2web_meetings_attach['und'] as $ref_bullet_point_attachment) {
          $attachment = node_load($ref_bullet_point_attachment['target_id']);
          if (empty($attachment->field_os2web_meetings_bpa_body['und'][0]['value']) && empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
            return FALSE;
          }
        }
      }
    }
  }

  // Going through all the descriptions.
  // Personal agenda.
  if (isset($meeting->field_os2web_meetings_full_doc_p[$meeting->language])) {
    $file_name = $meeting->field_os2web_meetings_full_doc_p[$meeting->language][0]['filename'];
    $file_name = explode('.', $file_name);
    $extention = strtoupper($file_name[1]);
    if (stripos($extention, 'pdf') !== 0) {
      return FALSE;
    }
  }
  // Closed agenda.
  if (isset($meeting->field_os2web_meetings_full_doc_c[$meeting->language])) {
    $file_name = $meeting->field_os2web_meetings_full_doc_c[$meeting->language][0]['filename'];
    $file_name = explode('.', $file_name);
    $extention = strtoupper($file_name[1]);
    if (stripos($extention, 'pdf') !== 0) {
      return FALSE;
    }
  }
  // Opened agenda.
  if (isset($meeting->field_os2web_meetings_full_doc[$meeting->language])) {
    $file_name = $meeting->field_os2web_meetings_full_doc[$meeting->language][0]['filename'];
    $file_name = explode('.', $file_name);
    $extention = strtoupper($file_name[1]);
    if (stripos($extention, 'pdf') !== 0) {
      return FALSE;
    }
  }
  // Additional personal agenda.
  if (isset($meeting->field_os2web_meetings_add_doc_p[$meeting->language])) {
    foreach ($meeting->field_os2web_meetings_add_doc_p[$meeting->language] as $description_field) {
      $file_name = $description_field['filename'];
      $file_name = explode('.', $file_name);
      $extention = strtoupper($file_name[1]);
      if (stripos($extention, 'pdf') !== 0) {
        return FALSE;
      }
    }
  }
  // Additional closed agenda.
  if (isset($meeting->field_os2web_meetings_add_doc_c[$meeting->language])) {
    foreach ($meeting->field_os2web_meetings_add_doc_c[$meeting->language] as $description_field) {
      $file_name = $description_field['filename'];
      $file_name = explode('.', $file_name);
      $extention = strtoupper($file_name[1]);
      if (stripos($extention, 'pdf') !== 0) {
        return FALSE;
      }
    }
  }
  // Additional opened agenda.
  if (isset($meeting->field_os2web_meetings_add_doc[$meeting->language])) {
    foreach ($meeting->field_os2web_meetings_add_doc[$meeting->language] as $description_field) {
      $file_name = $description_field['filename'];
      $file_name = explode('.', $file_name);
      $extention = strtoupper($file_name[1]);
      if (stripos($extention, 'pdf') !== 0) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/**
 * Determines if user's agent device is touch device (iPad or Android).
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_is_touch() {
  $isiPad = (bool) strpos($_SERVER['HTTP_USER_AGENT'], 'iPad');
  $isAndroid = (bool) strpos($_SERVER['HTTP_USER_AGENT'], 'Android');
  $isTouch = ($isiPad or $isAndroid);
  return $isiPad;
}

/**
 * Helper function to return fully loaded current user.
 *
 * @return object
 *   User object.
 */
function os2dagsorden_access_helper_get_user() {
  global $user;
  if (!isset($GLOBALS['full_user']) && $user->uid) {
    $GLOBALS['full_user'] = user_load($user->uid);
  }
  if (!$user->uid) {
    $GLOBALS['full_user'] = NULL;
  }

  return $GLOBALS['full_user'];
}

/**
 * This checks whether a certain meeting has closed bullets points.
 *
 * If at least one bullet point is closed, TRUE is returned.
 *
 * @param int $meeting_id
 *   Meeting node id.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_meeting_has_closed_bullet_point($meeting_id) {
  $meeting = node_load($meeting_id);
  if (isset($meeting->field_os2web_meetings_bullets['und'])) {
    foreach ($meeting->field_os2web_meetings_bullets['und'] as $bullet_point) {
      $bp_node = node_load($bullet_point);
      $is_closed = $bp_node->field_os2web_meetings_bul_closed['und'][0]['value'];
      $is_personal = $bp_node->field_os2web_meetings_bul_pers['und'][0]['value'];

      // Is closed or personal.
      if (intval($is_closed) == 1 || intval($is_personal) == 1) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
 * Helper function. Returns the committee based on its sub id.
 *
 * @param int $com_subid
 *   Committee sub id.
 *
 * @return term
 *   The committee term, if found.
 */
function _os2dagsorden_access_helper_get_committee_by_subid_and_name($com_subid, $name = NULL) {
  $vid = db_select('taxonomy_vocabulary', 'tv')
    ->fields('tv', array('vid'))
    ->condition('machine_name', 'os2web_meetings_tax_committee')
    ->execute()
    ->fetchField();

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'taxonomy_term')
    ->propertyCondition('vid', $vid)
    ->fieldCondition('field_os2web_meetings_com_subid', 'value', intval($com_subid), '=');

  $result = $query->execute();
  if (isset($result['taxonomy_term'])) {
    $tids = array_keys($result['taxonomy_term']);
    $tid = reset($tids);
    $term = taxonomy_term_load($tid);
    return $term;
  }
  elseif ($name) {
    $term = taxonomy_get_term_by_name($name, 'os2web_meetings_tax_committee');
    if (!empty($term)) {
      if (is_array($term)) {
        $term = array_pop($term);
      }
      $term->field_os2web_meetings_com_subid['und'][]['value'] = $com_subid;
      return $term;
    }
  }
  return FALSE;
}

/**
 * Callback function for sort methods.
 *
 * That compares the terms name using strnatcmp comparison.
 *
 * @param object $term1
 *   First term.
 * @param object $term2
 *   Second term.
 *
 * @return int
 *   Returns 0 if equals, -1 if the first is less tan second, 1 if first is
 *   greater than second.
 */
function os2dagsorden_access_helper_compare_term_by_name($term1, $term2) {
  return strnatcmp($term1->name, $term2->name);
}

/**
 * Function that fetches the term, or if that fails creates one.
 *
 * @param string $name
 *   Name of term.
 * @param string $vocabulary_name
 *   Name of vocabulary.
 *
 * @return object
 *   Fetched or created term.
 */
function os2dagsorden_access_helper_fetch_or_create_taxonomy_term($name, $vocabulary_name) {
  $terms = taxonomy_get_term_by_name($name, $vocabulary_name);
  if (!empty($terms)) {
    $term = array_pop($terms);
  }

  // Not found, create a new one.
  if (!$term) {
    $vid = db_select('taxonomy_vocabulary', 'tv')
      ->fields('tv', array('vid'))
      ->condition('machine_name', $vocabulary_name)
      ->execute()
      ->fetchField();

    $term = new stdClass();
    $term->vid = $vid;
    $term->name = $name;
    taxonomy_term_save($term);
  }
  // Return the fetched term or the one we have just created.
  return $term;
}

/**
 * Helper function to check if the user is admin of the certain committee.
 *
 * @param string $meeting_type
 *   Type of meeting.
 * @param int $tid
 *   Tid of committee.
 * @param int $uid
 *   Uid of user.
 *
 * @return bool
 *   True or false.
 */
function _os2dagsorden_access_helper_check_user_is_committee_admin($meeting_type, $tid, $uid) {
  // Always true for admin.
  if ($uid == 1) {
    return TRUE;
  }
  if (isset($tid)) {
    $term = taxonomy_term_load($tid);
    if (strcasecmp($meeting_type, 'Dagsorden') == 0) {
      $committee_admins = field_get_items('taxonomy_term', $term, 'field_os2web_meetings_com_dag_ad');
    }
    else {
      $committee_admins = field_get_items('taxonomy_term', $term, 'field_os2web_meetings_com_ref_ad');
    }

    if (isset($committee_admins) && !empty($committee_admins)) {
      foreach ($committee_admins as $key => $user) {
        if ($user['uid'] == $uid) {
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}

/**
 * Check PDF is enabled.
 *
 * Checks if the PDF download is available on this installation for user with
 * that IP address.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_access_helper_check_pdf_download_enabled() {
  if (variable_get('os2dagsorden_pdf_download_whitelist_enabled')) {
    $whitelisted_ips = explode(',', variable_get('os2dagsorden_pdf_download_whitelist'));
    $remote_ip = ip_address();

    foreach ($whitelisted_ips as $whitelisted_ip) {
      // If we the client request is from one of the IP's, login using
      // SimpleSAMLphp; otherwise use nemid login.
      //
      // If check for only parts of the ip address.
      // This makes it possible to add only the beginning of the IP range.
      // F.ex. 192.168 will allow all ip adresses including 192.168 as part of
      // the io.
      if (strpos($remote_ip, $whitelisted_ip) !== FALSE) {
        return TRUE;
      }
    }
    return FALSE;
  }

  return TRUE;
}

/**
 * Helper function.
 *
 * Function determines whether user should or should not be shown a special
 * Byradet link. Only members or followers+ will be shown the meeting.
 *
 * @return bool
 *   TRUE or FALSE.
 */
function os2dagsorden_access_helper_show_byradet_menu() {
  $user = os2dagsorden_access_helper_get_user();

  // Byradet committee only.
  $term = _os2dagsorden_importer_get_committee_by_id(4);

  // Hiding menu
  // is part of the committee.
  if (!empty($user->field_user_committee)) {
    foreach ($user->field_user_committee['und'] as $user_committee_id) {
      if (intval($term->tid) == $user_committee_id['tid']) {
        return TRUE;
      }
    }
  }

  // Follows committee PLUS.
  if (!empty($user->field_follows_committee_plus)) {
    foreach ($user->field_follows_committee_plus['und'] as $user_committee_id) {
      if (intval($term->tid) == $user_committee_id['tid']) {
        return TRUE;
      }
    }
  }

  // Follows committee PLUS.
  if (!empty($user->field_follows_committee_pp)) {
    foreach ($user->field_follows_committee_pp['und'] as $user_committee_id) {
      if (intval($term->tid) == $user_committee_id['tid']) {
        return TRUE;
      }
    }
  }

  return FALSE;
}
