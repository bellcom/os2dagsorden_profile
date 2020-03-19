<?php

/**
 * @file
 * os2web_meetings_content_types.ds.inc
 */

/**
 * Implements hook_ds_view_modes_info().
 */
function os2web_meetings_content_types_ds_view_modes_info() {
  $export = array();

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'os2dagsorden_meeting_overview';
  $ds_view_mode->label = 'OS2Dagsorden meeting overview';
  $ds_view_mode->entities = array(
    'node' => 'node',
  );
  $export['os2dagsorden_meeting_overview'] = $ds_view_mode;

  return $export;
}
