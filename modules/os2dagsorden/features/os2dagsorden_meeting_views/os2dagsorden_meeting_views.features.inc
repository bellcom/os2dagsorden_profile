<?php

/**
 * @file
 * os2dagsorden_meeting_views.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function os2dagsorden_meeting_views_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "ds" && $api == "ds") {
    return array("version" => "1");
  }
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function os2dagsorden_meeting_views_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}
