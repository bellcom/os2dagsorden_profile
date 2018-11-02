<?php

/**
 * @file
 * Code for the OS2Web Meeting Views - meetings search view.
 */

/**
 * Implement hook_breadcrumb().
 */
function os2dagsorden_meeting_views_meetings_search_breadcrumb() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $breadcrumb[] = l(t('Forsiden'), $base_url);
  $breadcrumb[] .= '<span class="breadcrumb-active">Møde søgning</span>';
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Meeting search help text.
 */
function os2dagsorden_meeting_views_meetings_search_help_text() {
  print '<div class="help-button" id="os2dagsorden_meeting_views_meetings_search_help" aria-label="' . variable_get('os2dagsorden_meetings_search_help_text', 'Her vises de møder der indeholdt møder,
  punkter eller bilag der matchede dine søgekriterier. Søgeresultatet vises på mødeniveau i listen.') . '"></div>';
}

/**
 * If the additional agenda should be filtered out.
 *
 * @param int $nid
 *   Meeting nid.
 *
 * @return bool
 *   True if removed, false it kept.
 */
function os2dagsorden_meeting_views_meetings_search_filter_additional_agendas($nid) {
  // Checking if the filtering is needed.
  if (!variable_get('os2dagsorden_meetings_search_include_add_agenda', TRUE)) {
    $meeting = node_load($nid);
    if (!empty($meeting->field_os2web_meetings_addendum['und']) && empty($_GET['search_params'])) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Prints bp id fields.
 *
 * @param mixed $meta_data
 *   Raw data from view.
 */
function os2dagsorden_meeting_views_meetings_search_fields_bp_id($meta_data) {
  $query = $_GET['search_params'];
  $full_meta_data = $meta_data;
  if (!empty($query)) {
    foreach (explode(" ", $query) as $search_param) {
      $search_string = $full_meta_data;
      while ($search_param_pos_s = stripos($search_string, $search_param)) {
        $full_search_string = $search_string;
        $search_param_pos_e = $search_param_pos_s + strlen($search_param);
        // Part of the text before the found occurence.
        $search_string = substr($search_string, 0, $search_param_pos_e + 1);

        $bp_id = _os2dagsorden_meeting_views_meetings_search_extract_bp_id($search_string);
        $bpa_id = _os2dagsorden_meeting_views_meetings_search_extract_bpa_id($search_string);

        if ($bp_id && $bpa_id) {
          print("/bullet-point/" . $bp_id);
          return;
        }
        $search_string = substr($full_search_string, $search_param_pos_e + 1);
      }
    }
  }
}

/**
 * Prints bpa id field.
 *
 * @param mixed $meta_data
 *   Raw data from view.
 */
function os2dagsorden_meeting_views_meetings_search_fields_bpa_id($meta_data) {
  $query = $_GET['search_params'];
  $full_meta_data = $meta_data;
  if (!empty($query)) {
    foreach (explode(" ", $query) as $search_param) {
      $search_string = $full_meta_data;
      while ($search_param_pos_s = stripos($search_string, $search_param)) {
        $full_search_string = $search_string;
        $search_param_pos_e = $search_param_pos_s + strlen($search_param);
        // Part of the text before the found occurence.
        $search_string = substr($search_string, 0, $search_param_pos_e + 1);

        $bp_id = _os2dagsorden_meeting_views_meetings_search_extract_bp_id($search_string);
        $bpa_id = _os2dagsorden_meeting_views_meetings_search_extract_bpa_id($search_string);

        if ($bp_id && $bpa_id) {
          print("/bullet-point-attachment/" . $bpa_id);
          return;
        }
        $search_string = substr($full_search_string, $search_param_pos_e + 1);
      }
    }
  }
}

/**
 * Prints search param.
 */
function os2dagsorden_meeting_views_meetings_search_fields_search_param() {
  $search_param = $_GET['search_params'];
  print('?search_param=' . $search_param);
}

/**
 * Prints speaker paper indicator.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meetings_search_fields_meeting_has_sp($meeting_id) {
  if (os2dagsorden_access_helper_meeting_has_speaker_paper($meeting_id)) {
    print '<div class="indicator-has-speaker-paper"></div>';
  }
  else {
    print '<div class="indicator-empty"></div>';
  }
}

/**
 * Prints speaker notes indicator.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meetings_search_fields_meeting_has_notes($meeting_id) {
  $annotations = os2dagsorden_annotator_get_notes_by_meeting_id($meeting_id);
  if (!empty($annotations)) {
    print '<div class="indicator-has-notes"></div>';
  }
  else {
    print '<div class="indicator-empty"></div>';
  }
}

/**
 * Prints closed indicator.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meetings_search_fields_meeting_is_closed($meeting_id) {
  if (variable_get('os2dagsorden_search_meeting_is_closed', FALSE)) {
    os2dagsorden_meeting_views_common_meeting_is_closed_else_empty($meeting_id);
  }
}

/**
 * Prints meeting title.
 *
 * @param mixed $data
 *   Meeting data.
 */
function os2dagsorden_meeting_views_meetings_search_fields_meeting_title($data) {
  os2dagsorden_meeting_views_common_meeting_title_listed($data);
}

/**
 * Prints meeting placement fields.
 *
 * @param mixed $meta_data
 *   Meeting data.
 */
function os2dagsorden_meeting_views_meetings_search_fields_placering($meta_data) {
  $query = $_GET['search_params'];
  $full_meta_data = $meta_data;
  if (!empty($query)) {
    foreach (explode(" ", $query) as $search_param) {
      $search_string = $full_meta_data;
      while ($search_param_pos_s = stripos($search_string, $search_param)) {
        $full_search_string = $search_string;
        $search_param_pos_e = $search_param_pos_s + strlen($search_param);
        // Part of the text before the found occurence.
        $search_string = substr($search_string, 0, $search_param_pos_e + 1);

        $bp_id = _os2dagsorden_meeting_views_meetings_search_extract_bp_id($search_string);
        $bpa_id = _os2dagsorden_meeting_views_meetings_search_extract_bpa_id($search_string);

        if ($bp_id && $bpa_id) {
          $bullet_point = node_load($bp_id);
          $bpa = node_load($bpa_id);

          print $bpa->title . ': [' . $bullet_point->title . ']';
          return;
        }
        $search_string = substr($full_search_string, $search_param_pos_e + 1);
      }
    }
  }
  print '&nbsp;';
}

/**
 * Extracts bullet point id from metadata.
 *
 * @param $meta_data
 *
 * @return string
 */
function _os2dagsorden_meeting_views_meetings_search_extract_bp_id($meta_data) {
  $search_token_pos_s = strripos($meta_data, '[search_token]');
  $search_token_pos_e = $search_token_pos_s + strlen('[search_token]');

  $bullet_point_token_pos_s = strripos($meta_data, '[bp]');
  $bullet_point_token_pos_e = $bullet_point_token_pos_s + strlen('[bp]');

  $bpa_token_pos_s = strripos($meta_data, '[bpa]');

  // Is in bullet point.
  if ($bullet_point_token_pos_s === $search_token_pos_e) {
    $bullet_point_id = substr($meta_data, $bullet_point_token_pos_e + 1, $bpa_token_pos_s - $bullet_point_token_pos_e - 2);
    return $bullet_point_id;
  }
}

/**
 * Extracts bullet point attachment id from metadata.
 *
 * @param $meta_data
 *
 * @return string
 */
function _os2dagsorden_meeting_views_meetings_search_extract_bpa_id($meta_data) {
  $search_token_pos_s = strripos($meta_data, '[search_token]');
  $search_token_pos_e = $search_token_pos_s + strlen('[search_token]');

  $bullet_point_token_pos_s = strripos($meta_data, '[bp]');

  $bpa_token_pos_s = strripos($meta_data, '[bpa]');
  $bpa_token_pos_e = $bpa_token_pos_s + strlen('[bpa]');

  // Is in bullet point.
  if ($bullet_point_token_pos_s === $search_token_pos_e) {
    $bpa_id = substr($meta_data, $bpa_token_pos_e + 1, strripos($meta_data, ']') - $bpa_token_pos_e - 1);
    return $bpa_id;
  }
}

/**
 * Prints meeting committee.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meetings_search_fields_committee($meeting_id) {
  if (variable_get('os2dagsorden_meetings_search_show_meeting_committee', TRUE)) {
    os2dagsorden_meeting_views_common_meeting_committee($meeting_id);
  }
}

/**
 * Prints meeting location.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meetings_search_fields_location($meeting_id) {
  if (variable_get('os2dagsorden_meetings_search_show_meeting_location', FALSE)) {
    os2dagsorden_meeting_views_common_meeting_location($meeting_id);
  }
}

/**
 * Prints meeting status.
 *
 * @param int $meeting_id
 *   Meeting nid.
 */
function os2dagsorden_meeting_views_meetings_search_fields_status($meeting_id) {
  if (variable_get('os2dagsorden_meetings_search_show_meeting_status', FALSE)) {
    $meeting = node_load($meeting_id);
    $meeting_type = field_get_items('node', $meeting, 'field_os2web_meetings_type');
    $options = field_info_field(field_os2web_meetings_type);

    print $options['settings']['allowed_values'][$meeting_type[0]['value']];
  }
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
function os2dagsorden_meeting_views_meetings_search_filter_meeting($meeting_id) {
  return os2dagsorden_meeting_views_common_filter_meeting($meeting_id);
}
