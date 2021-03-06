<?php

/**
 * @file
 * Code for the OS2Web Meeting Views - Seneste 8 moder view.
 */

/**
 * Prints last meetings help text.
 */
function os2dagsorden_meeting_views_last_8_meetings_help_text($text = '') {
  print '<div class="help-button" id="os2dagsorden_meeting_views_last_8_meetings_help' . '_' . $text . '" aria-label="' . variable_get('os2dagsorden_last_8_meetings_help_text', 'Her vises de seneste 8 møder i alle mødefora.
  Der er ikke et referat tilgængeligt før status er angivet til Referat eller Referat+.') . '"></div>';
}

/**
 * Customer access for all last meetings block.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_meeting_views_last_8_meetings_block_all_meetings_access() {
  if (!os2dagsorden_access_helper_get_user()) {
    return FALSE;
  }

  if (variable_get('os2dagsorden_latest_meetings_view_mode', 'all') === 'all') {
    return TRUE;
  }
  // Check if 'Mine' display returns no results - if that is the case, return 'All' display anyways.
  else {
    $view = views_get_view_result('seneste_8_m_der_i_de_st_ende_politiske_udvalg_', 'block_1');
    $result = count($view);
    if (!$result) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Customer access for my last meetings block.
 *
 * @return bool
 *   True or false.
 */
function os2dagsorden_meeting_views_last_8_meetings_block_my_meetings_access() {
  if (!os2dagsorden_access_helper_get_user()) {
    return FALSE;
  }
  if (variable_get('os2dagsorden_latest_meetings_view_mode', 'all') === 'mine') {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Filters out the non releavant meetings.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 *
 * @return bool
 *   True if filtered out, false is present.
 */
function os2dagsorden_meeting_views_last_8_meetings_block_my_meetings_filter($meeting_id) {
  // Get meeting committee tid.
  $meeting = node_load($meeting_id);
  $meeting_committee_tid = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
  // Get full user.
  $full_user = os2dagsorden_access_helper_get_user();

  // Check if user meeting committee in 'member of'.
  if (isset($full_user->field_user_committee['und'])) {
    foreach ($full_user->field_user_committee['und'] as $com) {
      if ($meeting_committee_tid === $com['tid']) {
        return FALSE;
      }
    }
  }
  // Check if user meeting committee in 'follows'.
  if (isset($full_user->field_follows_committee['und'])) {
    foreach ($full_user->field_follows_committee['und'] as $com) {
      if ($meeting_committee_tid === $com['tid']) {
        return FALSE;
      }
    }
  }
  // Check if user meeting committee in 'follows plus'.
  if (isset($full_user->field_follows_committee_plus['und'])) {
    foreach ($full_user->field_follows_committee_plus['und'] as $com) {
      if ($meeting_committee_tid === $com['tid']) {
        return FALSE;
      }
    }
  }

  // Check if user meeting committee in 'follows plus plus'.
  if (isset($full_user->field_follows_committee_pp['und'])) {
    foreach ($full_user->field_follows_committee_pp['und'] as $com) {
      if ($meeting_committee_tid === $com['tid']) {
        return FALSE;
      }
    }
  }

  // None of the above, filter entry out.
  return TRUE;
}

/**
 * Prints speaker paper indicator.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_last_8_meetings_fields_meeting_has_sp($meeting_id) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_title', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_has_sp_else_empty($meeting_id);
  }
}

/**
 * Prints speaker notes indicator.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_last_8_meetings_fields_meeting_has_notes($meeting_id) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_title', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_has_notes_else_empty($meeting_id);
  }
}

/**
 * Prints speaker closed indicator.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_last_8_meetings_fields_meeting_is_closed($meeting_id) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_is_closed', FALSE)) {
    os2dagsorden_meeting_views_common_meeting_is_closed_else_empty($meeting_id);
  }
}

/**
 * Prints meeting title.
 *
 * @param string $data
 *   Meeting data.
 */
function os2dagsorden_meeting_views_last_8_meetings_fields_meeting_title($data) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_title', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_title_listed($data);
  }
}

/**
 * Prints meeting committee.
 *
 * @param string $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_last_8_meetings_fields_meeting_committee($meeting_id) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_committee', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_committee($meeting_id);
  }
}

/**
 * Prints meeting location.
 *
 * @param string $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_last_8_meetings_fields_meeting_location($meeting_id) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_location', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_location($meeting_id);
  }
}

/**
 * Filter function to show only relevant committees for the user.
 *
 * @param int $tid
 *   Tid of the committees.
 */
function os2dagsorden_meeting_views_last_8_meetings_filter_in_my_list($tid) {
  return os2dagsorden_meeting_views_common_filter_in_my_list($tid);
}

/**
 * Filter function to show only relevant committees for the user.
 *
 * @param int $tid
 *   Tid of the committees.
 */
function os2dagsorden_meeting_views_last_8_meetings_all_meetings_filter_in_my_list($nid, $tid) {
  $user = os2dagsorden_access_helper_get_user();
  // External user.
  if (isset($user->field_user_external['und']) && (int) $user->field_user_external['und'][0]['value'] === 1) {
    return !(in_array($tid, os2dagsorden_access_helper_get_user_member_of_committees()) && !os2dagsorden_meeting_views_common_filter_meeting($nid));
  }
  return !os2dagsorden_access_helper_check_user_can_see_meeting(null, $tid);
}

/**
 * Decides whether user has view access to the following node.
 *
 * @param int $meeting_id
 *   Id of the meeting.
 *
 * @return bool
 *   False is user has access, true if user does not have access.
 */
function os2dagsorden_meeting_views_last_8_meetings_filter_meeting($meeting_id) {
  return os2dagsorden_meeting_views_common_filter_meeting($meeting_id);
}
