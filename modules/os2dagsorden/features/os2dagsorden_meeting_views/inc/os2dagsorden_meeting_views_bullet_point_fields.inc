<?php

/**
 * @file
 * Code for the OS2Web Meeting Views - speaker paper (bullet point) view.
 */

/**
 * Implements hook_breadcrumb().
 */
function os2dagsorden_meeting_views_bullet_point_breadcrumb() {
  global $base_url;
  $q = explode('/', $_GET['q']);
  $breadcrumb[] = l(t('Forsiden'), $base_url);
  $breadcrumb[] .= l(t('Mødedetaljer'), 'meeting/' . $q[1]);
  $breadcrumb[] .= '<span class="breadcrumb-active">Dagsordenspunkt</span>';
  drupal_set_breadcrumb($breadcrumb);
}

/**
 * Prints send to friend link.
 */
function os2dagsorden_meeting_views_bullet_point_print_send_to_friend() {
  if (module_exists('os2web_print_send_to_friend')) {
    if (os2dagsorden_access_helper_check_pdf_download_enabled()) {
      global $base_url;
      $q = explode('/', $_GET['q']);
      $meeting_id = $q[1];
      $bp_id = $q[3];
      print '<div class="controls-right">';
      print '<a href="' . $base_url . '?q=/print/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/print" class="form-submit print-button" target="_blank">Print</a>';
      print '</div>';
      if (os2dagsorden_access_helper_check_user_can_send_bullet_point($meeting_id, $bp_id)) {
        print '<div class="controls-right">';
        print '<a href="' . $base_url . '?q=meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/send_to_friend_form" class="form-submit" rel="lightframe">Send til en ven</a>';
        print '</div>';

      }
      print '<div class="clearfix"></div>';
    }
  }
}

/**
 * Check if user can see bullet point.
 *
 * @return bool
 *   True of false.
 */
function os2dagsorden_meeting_views_bullet_point_page_access() {
  $q = explode("/", $_GET['q']);

  $meeting_id = $q[1];
  $bullet_point_id = $q[3];

  return os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $bullet_point_id);
}

/**
 * Wrapper to call to os2dagsorden_meeting_views_common_bullet_point_has_notes.
 *
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_bullet_point_fields_indicator_bullet_point_has_notes($bp_id) {
  os2dagsorden_meeting_views_common_bullet_point_has_notes($bp_id);
}

/**
 * Prints navigation buttons to jump between bullet points.
 *
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_bullet_point_fields_title_navigation_buttons($bp_id) {
  $q = explode("/", $_GET['q']);

  $previous_bullet_point = NULL;
  $next_bullet_point = NULL;

  $meeting_id = $q[1];
  $all_meetings = os2dagsorden_access_helper_get_meeting_and_tillaegs_meetings($meeting_id);
  foreach ($all_meetings as $nid) {
    $meeting = node_load($nid);

    $bullet_point = node_load($bp_id);

    if (!empty($meeting->field_os2web_meetings_bullets['und'])) {
      foreach ($meeting->field_os2web_meetings_bullets['und'] as $new_bp) {
        if (intval($bp_id) != intval($new_bp['target_id'])) {
          $new_bullet_point = node_load($new_bp['target_id']);
          // Checking if the ordinal number of new bullet point is BELOW the current bulet point ordinal number
          // at the same time, checking if user has an access to that bullet point.
          if ($new_bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] < $bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] && os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $new_bp['target_id'])) {
            // Checking if there is no pointer to previous bullet point
            // or the ordinal number of the new bullet point is higher than the ordinal number of current previous bullet point.
            if (!$previous_bullet_point || $new_bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] > $previous_bullet_point->field_bul_point_number['und'][0]['value']) {
              $previous_bullet_point = $new_bullet_point;
            }
          }
          // Checking if the ordinal number of new bullet point is ABOVE the current bulet point ordinal number
          // at the same time, checking if user has an access to that bullet point.
          elseif ($new_bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] > $bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] && os2dagsorden_access_helper_check_user_can_see_bullet_point($meeting_id, $new_bp['target_id'])) {
            // Checking if there is no pointer to next bullet point
            // or the ordinal number of the new bullet point is higher than the ordinal number of current next bullet point.
            if (!$next_bullet_point || $new_bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] < $next_bullet_point->field_bul_point_number['und'][0]['value']) {
              $next_bullet_point = $new_bullet_point;
            }
          }

          // Check if everything is set already, then break the loop.
          if ($previous_bullet_point && $next_bullet_point &&
            $previous_bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] + 1 ==
            $bullet_point->field_os2web_meetings_bul_number['und'][0]['value']
            &&
            $next_bullet_point->field_os2web_meetings_bul_number['und'][0]['value'] - 1 ==
            $bullet_point->field_os2web_meetings_bul_number['und'][0]['value']
          ) {
            break;
          }
        }
      }
    }
  }

  global $base_url;

  if ($previous_bullet_point) {
    print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $previous_bullet_point->nid . '" class="form-submit nav-button left"><</a>';
  }
  if ($next_bullet_point) {
    print '<a href="' . $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $next_bullet_point->nid . '" class="form-submit nav-button right">></a>';
  }

  if ($bullet_point->field_os2web_meetings_bul_closed['und'][0]['value'] == 1) {
    print '<div class="indicator-unlocked"></div>';
  }
  if ($bullet_point->field_os2web_meetings_bul_pers['und'][0]['value'] == 1) {
    print '<div class="indicator-personal"></div>';
  }

  $title = $bullet_point->title;
  if (variable_get('os2dagsorden_show_bullet_case_nr', FALSE)) {
    $bp = node_load($bp_id);
    if ($bp->field_os2web_meetings_bul_case['und'] && !empty($bp->field_os2web_meetings_bul_case['und'][0]['value'])) {
      $title .= ' (' . $bp->field_os2web_meetings_bul_case['und'][0]['value'] . ')';
    }
  }
  print $title;
}

/**
 * Prints bullet point fields and content.
 *
 * @param int $meeting_id
 *   Nid of meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function os2dagsorden_meeting_views_bullet_point_fields_attachments_and_content($meeting_id, $bp_id) {
  global $base_url;
  $bullet_point = node_load($bp_id);
  if (isset($bullet_point->field_os2web_meetings_attach['und'])) {
    print '<table><tr><td>';
    print '<div class="item-list item-list-dagsordenspunkt">';
    print '<ul class="ul-item-list-dagsordenspunkt pos-rel">';

    $bilags = array();

    $all_attachments = array();
    foreach ($bullet_point->field_os2web_meetings_attach['und'] as $attachment_id) {
      $hash = $attachment_id['target_id'];
      $all_attachments[$hash] = $attachment_id;
    }

    foreach ($all_attachments as $attachment_id) {
      $attachment = node_load($attachment_id['target_id']);

      // If is bilag.
      if (!empty($attachment->field_os2web_meetings_bpa_file['und'])) {
        // Saving bilags to the end.
        if ($attachment->field_os2web_meetings_bpa_type['und'][0]['value'] === 'Sager') {
          $cases[] = $attachment;
        }
        else {
          $bilags[] = $attachment;
        }
      }
      // Not bilag.
      else {
        print '<li class="non-bilag">';

        $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
        print '<a href="' . $link . '" class="first-child last-child"><div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

        // Attachment text - if any.
        if (!empty($attachment->field_os2web_meetings_bpa_body['und'][0]['value'])) {
          print '<div class="attachment_text_container">';

          print '<div class="attachment_text_full_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';
          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';

          print '</div>';
        }

        print '</li>';
      }
    }

    if ($bullet_point->title !== 'Bilag' && $bullet_point->title !== 'Sager') {
      if (!empty($cases) && !(variable_get('os2dagsorden_show_case_bullet_point', FALSE)) && variable_get('os2dagsorden_show_bilag_bullet_point', FALSE)) {
        _os2dagsorden_meeting_views_bullet_point_bilag_cases($cases, $meeting_id, $bp_id);
        if (!empty($bilags)) {
          _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point('Bilag', $bilags, $meeting_id, $bp_id, 'bilags');
        }
      }
      else {
        if (!empty($bilags)) {
          if (variable_get('os2dagsorden_show_bilag_bullet_point', FALSE)) {
            _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point('Bilag', $bilags, $meeting_id, $bp_id, 'bilags');
          }
          else {
            _os2dagsorden_meeting_views_bullet_point_bilag_cases($bilags, $meeting_id, $bp_id);
          }
        }
        if (!empty($cases)) {
          if (variable_get('os2dagsorden_show_case_bullet_point', FALSE)) {
            _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point(variable_get('os2dagsorden_sager_group_title', 'Sager'), $cases, $meeting_id, $bp_id, 'cases');
          }
          else {
            _os2dagsorden_meeting_views_bullet_point_bilag_cases($cases, $meeting_id, $bp_id);
          }

        }
      }
    }
    else {
      _os2dagsorden_meeting_views_bullet_point_bilag_cases_with_content($bilags, $meeting_id, $bp_id);
      _os2dagsorden_meeting_views_bullet_point_bilag_cases_with_content($cases, $meeting_id, $bp_id);
    }
    print '</ul>';
    print '</div>';
    print '</td></tr></table>';
  }
}

/**
 * Prints bullet point cases.
 *
 * @param mixed $elements
 *   Attachments.
 * @param int $meeting_id
 *   Nid of meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function _os2dagsorden_meeting_views_bullet_point_bilag_cases($elements, $meeting_id, $bp_id) {
  global $base_url;
  foreach ($elements as $attachment) {
    print '<li class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    }
    else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';
      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }
      else {
        print '<div class="indicator-empty"></div>';
      }

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

      // Attachment text - if any.
      if (variable_get('os2dagsorden_show_bilag', TRUE)) {
        if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
          print '<div class="attachment_text_container">';

          print '<div class="attachment_text_trimmed_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';

          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';
          print('<a class="first-child last-child" href="' . $link . '" target="_blank">Læs mere...</a>');

          print '</div>';
        }
      }
    }
    else {
      print '<a class="first-child last-child">';
      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }
      else {
        print '<div class="indicator-empty"></div>';
      }

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';
    }
    if (variable_get('os2dagsorden_show_bilag_pages_amount', FALSE)) {
      if ((int) $attachment->field_os2web_meetings_bpa_pages['und'][0]['value'] > 1) {
        print '<div class = "bilag-pages">' . t(' @pages sider', array('@pages' => $attachment->field_os2web_meetings_bpa_pages['und'][0]['value'])) . '</div>' ;
      }
      else {
        print '<div class = "bilag-pages">' . t(' @pages side', array('@pages' => $attachment->field_os2web_meetings_bpa_pages['und'][0]['value'])) . '</div>' ;
      }
    }
    print '</li>';
  }
}

/**
 * Prints bullet point cases.
 *
 * @param string $bp_title
 *   Title of bullet point.
 * @param mixed $elements
 *   Attachments.
 * @param int $meeting_id
 *   Nid of meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 * @param string $attachment_type
 *   Type of the attachment.
 */
function _os2dagsorden_meeting_views_bullet_point_bilag_cases_bullet_point($bp_title, $elements, $meeting_id, $bp_id, $attachment_type) {
  global $base_url;
  print '<li class="bilags_cases ' . $attachment_type . '">';
  print '<div class="indicator-has-no-notes"></div>';
  print '<a class="first-child last-child">' . $bp_title . '</a>';
  print '<div class="bilags_cases_container">';

  // Going through all bilags.
  foreach ($elements as $attachment) {
    print '<div class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    }
    else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';
    }
    else {
      print '<a class="first-child last-child">';
    }

    if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
      print '<div class="indicator-unlocked"></div>';
    }
    else {
      print '<div class="indicator-empty"></div>';
    }
    print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

    print '</div>';
  }
  print '</div>';
  print '</li>';
}

/**
 * Prints the bullet point with attachments.
 *
 * @param mixed $elements
 *   Array of attachments.
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of bullet point.
 */
function _os2dagsorden_meeting_views_bullet_point_bilag_cases_with_content($elements, $meeting_id, $bp_id) {
  global $base_url;
  // Going through all bilags.
  foreach ($elements as $attachment) {
    print '<li class="bilag">';

    if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
      $link = $base_url . '/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/bullet-point-attachment/' . $attachment->nid;
    }
    else {
      $link = file_create_url($attachment->field_os2web_meetings_bpa_file['und'][0]['uri']);
    }

    if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bp_id, $attachment->nid)) {
      print '<a href="' . $link . '" target="_blank" class="first-child last-child">';

      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }

      print '<div id="' . $attachment->nid . '" class="indicator-has-no-notes"></div>' . $attachment->title . '</a>';

      // Attachment text - if any.
      if (!empty($attachment->field_os2web_meetings_bpa_html['und'][0])) {
        // Should bilag be visible.
        if (variable_get('os2dagsorden_show_bilag', TRUE)) {
          print '<div class="attachment_text_container">';

          print '<div class="attachment_text_trimmed_container ajax-progress bpa-' . $meeting_id . '-' . $bp_id . '-' . $attachment->nid . '">';

          print 'Vent venligst...<div class="throbber"></div>';
          print '</div>';
          print('<a class="first-child last-child" href="' . $link . '" target="_blank">Læs mere...</a>');

          print '</div>';
        }
      }
    }
    else {
      print '<a class="first-child last-child">';

      if ($attachment->field_os2web_meetings_bpa_closed['und'][0]['value'] == 1) {
        print '<div class="indicator-unlocked"></div>';
      }

      print $attachment->title . '</a>';
    }
    print '</li>';
  }
}

/**
 * Prints print link for speaker paper.
 *
 * @param int $bp_id
 *   Nid of bullet point.
 * @param int $sp_id
 *   Nid of speaker paper.
 */
function os2dagsorden_meeting_views_bullet_point_fields_print_link($bp_id, $sp_id) {
  global $base_url;
  print "<div class='speaker-paper-print'>";
  print '<input class="form-submit print-button" type="button" onclick="window.open(';
  print(" ' " . $base_url . '/print/bullet-point/' . $bp_id . '/speaker-paper/' . $sp_id . '/print' . " ' ");
  print ')" value="' . t('Print @speaker_paper', array('@speaker_paper' => strtolower(variable_get('os2dagsorden_title_meeting_comments', 'Talepapir')))) . '">';
  print "</div>";
}

/**
 * Prints a link to create new speaker paper for a specific bullet point.
 */
function os2dagsorden_meeting_views_bullet_point_fields_create_new_speaker_paper() {
  global $base_url;
  $q = $_GET['q'];
  $q_exp = explode('/', $q);
  $bullet_point = node_load($q_exp[3]);

  $link = $base_url . '/node/add/os2web-meetings-spaper?edit[field_os2web_meetings_sp_bullet][und][0][target_id]=' . $bullet_point->title . ' (' . $bullet_point->nid . ')&destination=' . $q;

  $text = variable_get('os2dagsorden_title_meeting_new_speaker_paper', 'Nyt talepapir');
  if (!variable_get('os2dagsorden_title_meeting_new_speaker_paper_hide', FALSE)) {
    print '<a href="' . $link . '" title="' . $text . '"><div class="indicator-create-speaker-paper"></div>' . $text . '</a>';
  }
  else {
    print '<a href="' . $link . '" title="' . $text . '"><div class="indicator-create-speaker-paper"></div>&nbsp;</a>';
  }
}

/**
 * Prints a list of names or groups which speaker paper is shared with.
 *
 * @param int $sp_id
 *   Nid of the speaker paper.
 */
function os2dagsorden_meeting_views_bullet_point_fields_shared_with($sp_id) {
  if (variable_get('os2dagsorden_shared_speaker_paper', TRUE)) {
    $sp = node_load($sp_id);
    $shared_with_html = NULL;

    $shared_with_str = NULL;
    $shared_with_group_str = NULL;
    if (isset($sp->field_os2web_meetings_sp_shared['und'])) {
      foreach ($sp->field_os2web_meetings_sp_shared['und'] as $shared_with) {
        $shared_with_user = user_load($shared_with['target_id']);
        $shared_with_str .= ($shared_with_user->field_user_full_name['und']) ? $shared_with_user->field_user_full_name['und'][0]['value'] : $shared_with_user->name;

        $shared_with_str .= ', ';
      }
    }
    if (isset($sp->field_os2web_meetings_sp_shar_gr['und'])) {
      foreach ($sp->field_os2web_meetings_sp_shar_gr['und'] as $shared_with) {
        $shared_with_group = taxonomy_term_load($shared_with['tid']);
        $shared_with_group_str .= $shared_with_group->name;
        $shared_with_group_str .= ', ';
      }
    }
    if (isset($shared_with_str)) {
      $shared_with_html = t('Deles med') . ': ' . substr($shared_with_str, 0, -2);
    }
    if (isset($shared_with_group_str)) {
      if (isset($shared_with_html)) {
        $shared_with_html .= '<br/>';
      }
      $shared_with_html .= t('Deles med grupper') . ': ' . substr($shared_with_group_str, 0, -2);
    }

    print $shared_with_html;
  }
}

/**
 * Filter function, inverts the call of os2dagsorden_access_helper_check_user_can_see_speaker_paper.
 *
 * @param int $sp_id
 *   Nid of the speaker paper.
 *
 * @return bool
 *   False is user can see speaker paper, true if cannot.
 */
function os2dagsorden_meeting_views_bullet_point_fields_filter($sp_id) {
  return !os2dagsorden_access_helper_check_user_can_see_speaker_paper($sp_id, variable_get('os2dagsorden_shared_speaker_paper', TRUE));
}

/**
 * Generates link for edit bullet point.
 *
 * @param int $meeting_id
 *   Nid of the meeting.
 * @param int $bp_id
 *   Nid of the bullet point.
 */
function os2dagsorden_meeting_views_bullet_point_modify_links($meeting_id, $bp_id) {
  if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
    print '<a href="' . $GLOBALS['base_url'] . '/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '" class="edit-meeting-btn" rel="lightframe[|width:800px; height:500px; scrolling: auto;]">'
        . '<div class="indicator-edit-agenda"></div>&nbsp;</a>'
        . '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/delete" class="edit-meeting-btn form-submit-delete">'
        . '<div class="indicator-delete-agenda"></div>&nbsp;</a>'
        . '<a href="/agenda/meeting/' . $meeting_id . '/bullet-point/' . $bp_id . '/attachment" class="edit-meeting-btn" rel="lightframe[|width:800px; height:500px; scrolling: auto;]">'
        . '<div class="indicator-add-agenda"></div>&nbsp;</a>';
  }
}
