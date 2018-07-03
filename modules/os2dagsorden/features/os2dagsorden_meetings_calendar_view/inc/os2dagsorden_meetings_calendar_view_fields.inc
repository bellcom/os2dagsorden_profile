<?php

/**
 * @file
 * Code for the OS2Web Calendar views - Events view.
 */

/**
 * Implement hook_breadcrumb().
 */
function os2dagsorden_meetings_calendar_view_breadcrumb() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $breadcrumb[] = l(t('Forsiden'), $base_url);
  $breadcrumb[] .= '<span class="breadcrumb-active">Kalender</span>';
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Prints meeting title.
 *
 * @param mixed $data
 *   Raw data from view.
 */
function os2dagsorden_meetings_calendar_view_fields_title($data) {
  $title = $data->_field_data['nid']['entity']->title;

  if (variable_get('os2dagsorden_meeting_view_calendar_short_name', FALSE)) {
    $committee = $data->_field_data['taxonomy_term_data_field_data_field_os2web_meetings_committe']['entity'];
    if ($committee->field_os2web_meetings_com_shname && is_array($committee->field_os2web_meetings_com_shname['und'])) {
      $title = $committee->field_os2web_meetings_com_shname['und'][0]['value'];
    }
  }

  $meeting_category = $data->_field_data['taxonomy_term_data_field_data_field_os2web_meetings_committe']['entity']->field_os2web_meetings_com_categ['und'][0]['tid'];
  if ($meeting_category) {
    $meeting_category = taxonomy_term_load($meeting_category);
    $meeting_category = substr(strtoupper($meeting_category->name), 0, 1);
  }

  if (variable_get('os2dagsorden_show_meeting_category', 'true') === 'true') {
    echo '<font style="color:#' . $data->_field_data['taxonomy_term_data_field_data_field_os2web_meetings_committe']['entity']->field_os2web_meetings_com_color['und'][0]['jquery_colorpicker'] . '">' . '[' . $meeting_category . '] ' . $title . '</font>';
  }
  else {
    echo '<font style="color:#' . $data->_field_data['taxonomy_term_data_field_data_field_os2web_meetings_committe']['entity']->field_os2web_meetings_com_color['und'][0]['jquery_colorpicker'] . '">' . $title . '</font>';
  }
}

/**
 * Filter meeting out by category.
 *
 * @param int $meeting_id
 *   Meeting nid.
 *
 * @return bool
 *   True if filtered out, false otherwise.
 */
function os2dagsorden_meetings_calendar_view_fields_filter_by_meeting_category($meeting_id) {
  $user_loaded = os2dagsorden_access_helper_get_user();

  $meeting = node_load($meeting_id);
  $committee = taxonomy_term_load($meeting->field_os2web_meetings_committee['und'][0]['tid']);
  $meeting_category = $committee->field_os2web_meetings_com_categ['und'][0]['tid'];
  if (isset($user_loaded->field_user_com_categ['und'])) {
    foreach ($user_loaded->field_user_com_categ['und'] as $user_category) {
      if ($user_category['tid'] === $meeting_category) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

/**
 * Filter if user cannot see meeting.
 *
 * @param int $meeting_id
 *   Meeting nid.
 *
 * @return bool
 *   True if filtered out, false otherwise.
 */
function os2dagsorden_meetings_calendar_view_fields_filter_for_os2dagsorden_limited($meeting_id) {
  $meeting = node_load($meeting_id);
  return !node_access('view', $meeting);
}

/**
 * Returns the month formatted date.
 *
 * @return string
 *   Date from query or the current date.
 */
function os2dagsorden_meetings_calendar_view_fields_month_date_contextual_filter() {
  $q = $_GET['q'];

  if (strlen($q) > strlen('calendar-field-date/month/')) {
    return substr($q, strrpos($q, 'month/') + 6, 7);
  }
  else {
    return date('Y-m');
  }
}

/**
 * Returns the day formatted date.
 *
 * @return string
 *   Date from query or the current date.
 */
function os2dagsorden_meetings_calendar_view_fields_day_date_contextual_filter() {
  $q = $_GET['q'];

  if (strlen($q) > strlen('calendar-field-date/day/')) {
    return substr($q, strrpos($q, 'day/') + 4, 10);
  }
  else {
    return date('Y-m-d');
  }
}

/**
 * Returns the month formatted date.
 *
 * @return string
 *   Date from query or the current date.
 */
function os2dagsorden_meetings_calendar_view_fields_block_date_contextual_filter() {
  $q = $_GET['q'];

  if (strrpos($q, 'calendar-field-date/day/') > -1 && strlen($q) > strlen('calendar-field-date/day/')) {
    return substr($q, strrpos($q, 'day/') + 4, 7);
  }
  else {
    return date('Y-m');
  }
}

/**
 * Calendar help text.
 */
function os2dagsorden_meetings_calendar_view_block_help_text() {
  print '<div class="help-button" id="os2dagsorden_meetings_calendar_view_block_help" aria-label="' . variable_get('os2dagsorden_calendar_help_text', 'Her findes en dags- eller månedskalender med links til dagsordner og referater hvis de er publicerede. Det er muligt at eksportere møderne til en fil, som så kan importeres til egen kalender, eks. i Outlook.') . '"></div>';
}
