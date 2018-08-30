<?php

/**
 * @file
 * Code for the OS2Web Meeting Views - Kommende moder i mine udvalg.
 */

/**
 * Prints coming meetings help text.
 */
function os2dagsorden_meeting_views_coming_meetings_help_text() {
  print '<div class="help-button" id="os2dagsorden_meeting_views_coming_meetings_help" aria-label="' . variable_get('os2dagsorden_upcoming_meetings_help_text', 'Her vises de kommende 5 møder i "Mine udvalg".
  Det vil sige udvalg som man enten er medlem af eller følger. Udvalgene kan ses i boksene 
  "Medlem af" og "Jeg følger" i højremenuen. Der er ikke en dagsorden tilgængelig før 
  status er angivet til Dagsorden eller Dagsorden+.') . '"></div>';
}

/**
 * Prints speaker paper indicator.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_coming_meetings_fields_meeting_has_sp($meeting_id) {
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
function os2dagsorden_meeting_views_coming_meetings_fields_meeting_has_notes($meeting_id) {
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
function os2dagsorden_meeting_views_coming_meetings_fields_meeting_is_closed($meeting_id) {
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
function os2dagsorden_meeting_views_coming_meetings_fields_meeting_title($data) {
  if (variable_get('os2dagsorden_meetings_views_last_8_show_meeting_title', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_title_listed($data);
  }
}

/**
 * Prints meeting location.
 *
 * @param string $meeting_id
 *   Nid of meeting.
 */
function os2dagsorden_meeting_views_coming_meetings_fields_meeting_location($meeting_id) {
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
function os2dagsorden_meeting_views_coming_meetings_filter_in_my_list($tid) {
  return os2dagsorden_meeting_views_common_filter_in_my_list($tid);
}

/**
 * Filter function to show only relevant meetings.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 *
 * @return bool
 *   False is user has access, true if user does not have access.
 */
function os2dagsorden_meeting_views_coming_meetings_filter_meeting($meeting_id) {
  return os2dagsorden_meeting_views_common_filter_meeting($meeting_id);
}