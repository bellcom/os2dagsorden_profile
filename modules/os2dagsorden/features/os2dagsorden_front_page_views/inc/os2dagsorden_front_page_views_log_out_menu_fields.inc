<?php

/**
 * @file
 * Code for the OS2Web front page views feature - Log out menu.
 */

/**
 * Generates link for logging off.
 */
function os2dagsorden_front_page_views_fields_text_log_off() {
  if (module_exists('os2intra_simplesaml')) {
    $logout_url = $GLOBALS['base_url'] . '/os2intra/simplesaml/logout';
  }
  else {
    $logout_url = $GLOBALS['base_url'] . '/user/logout';
  }

  print '<a href="' . $logout_url . '" class="user-logout-btn button">' . variable_get('os2dagsorden_title_general_log_off', 'Log af') . '</a>';
}

/**
 * Generates link for add meeting.
 */
function os2dagsorden_front_page_views_fields_create_meering() {
  if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
    print '<a href="' . $GLOBALS['base_url'] . '/agenda/meeting" class="create-meeting-btn button" rel="lightframe[|width:800px; height:500px; scrolling: auto;]">' . variable_get('os2dagsorden_title_general_create_meeting', 'Tilføj møde') . '</a>';
  }
}

/**
 * Generates link to the settings page.
 */
function os2dagsorden_front_page_views_fields_settings() {
  print '<a href="' . $GLOBALS['base_url'] . '/user/simple_edit" class="settings-btn button">' . variable_get('os2dagsorden_title_general_mine_indstillinger', 'Mine indstillinger') . '</a>';
}
