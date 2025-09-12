<?php

/**
 * @file
 * This file implements various debugging methods available from drush.
 */

/**
 * Implements hook_drush_command().
 */
function os2web_acadre_esdh_drush_command() {
  $items = array();
  $items['acadre-case-docs'] = array(
    'description' => 'Look up a case in the CM system, and list the included dokuments',
    'arguments' => array(
      'caseid' => 'The id of the case. Either the year/serial (ie. 11/4949) notation og the database case id',
    ),
    'aliases' => array('esdh-cm-docs'),
  );
  $items['acadre-case'] = array(
    'description' => 'Look up a case in the CM system, and list the included dokuments',
    'arguments' => array(
      'caseid' => 'The id of the case. Either the year/serial (ie. 11/4949) notation og the database case id',
    ),
    'aliases' => array('esdh-cm-case'),
  );
  $items['acadre-doc'] = array(
    'description' => 'Look up a document in the CM system, and list the included dokuments',
    'arguments' => array(
      'docid' => 'The id of the document. Either the serial/year (ie. 4949/11) notation og the database case id',
    ),
    'aliases' => array('esdh-cm-doc'),
  );
  $items['acadre-case-docs-load'] = array(
    'description' => 'Look up a case in the CM system, and load the included dokuments',
    'arguments' => array(
      'caseid' => 'The id of the case. Either the year/serial (ie. 11/4949) notation og the database case id',
    ),
    'aliases' => array('esdh-cm-docs-load'),
  );
  $items['acadre-cm-status'] = array(
    'description' => 'Check the status of the required webservices for CM',
    'arguments' => array(),
    'aliases' => array('esdh-st'),
  );
  $items['acadre-addenum-fix'] = array(
    'description' => 'Reorders addenums',
    'arguments' => array(),
    'aliases' => array('esdh-af'),
  );
  $items['os2web-acadre-esdh-delete-unused-dir'] = [
    'description' => 'Delete unused acadre directories under the given parent path.',
    'arguments' => [
      'dir' => 'The parent directory where to look for acadre folders.',
    ],
    'options' => [
      'dry-run' => 'Do not delete directories, just print what would be deleted.',
      'quiet'   => 'Suppress normal output.',
      'q'       => 'Alias for --quiet.',
    ],
    'examples' => [
      'drush os2web-acadre-esdh-delete-unused-dir /var/www/private/upload' => 'Scan and delete unused directories.',
      'drush os2web-acadre-esdh-delete-unused-dir /var/www/private/upload --dry-run' => 'Only show unused directories, do not delete.',
    ],
  ];

  return $items;
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_acadre_doc($docid = NULL) {
  if (os2web_esdh_provider_has_api('cm')) {
    drush_log('API load', 'ok');
    $data = os2web_esdh_provider_invoke('cm', 'get_document', $docid);
    if ($data === FALSE) {
      drush_log('Document lookup..', 'fail');
    }
    else {
      drush_log('Document lookup..', 'ok');
      error_log(print_r($data, 1));
    }
  }
  else {
    drush_log('Api load', 'fail');
  }
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_acadre_case($caseid = NULL) {
  if (os2web_esdh_provider_has_api('cm')) {
    $data = os2web_esdh_provider_invoke('cm', 'get_case_data', $caseid);
    drush_log('API load', 'ok');
    if ($data === FALSE) {
      drush_log('Case lookup..', 'fail');
    }
    else {
      drush_log('Case lookup..', 'ok');
      error_log(print_r($data, 1));
    }
  }
  else {
    drush_log('Api load', 'fail');
  }
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_acadre_case_docs($caseid = NULL) {
  if (os2web_esdh_provider_has_api('cm')) {
    drush_log('API load', 'ok');
    $data = os2web_esdh_provider_invoke('cm', 'get_case_docs', $caseid);
    if ($data === FALSE) {
      drush_log('Case lookup..', 'fail');
    }
    else {
      drush_log('Case lookup..', 'ok');
      error_log(print_r($data, 1));
    }
  }
  else {
    drush_log('Api load', 'fail');
  }
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_acadre_case_docs_load($caseid = NULL) {
  if (os2web_esdh_provider_has_api('cm')) {
    drush_log('API load', 'ok');
    $data = os2web_esdh_provider_invoke('cm', 'get_case_docs', $caseid);
    if ($data === FALSE) {
      drush_log('Case lookup..', 'fail');
    }
    else {
      drush_log('Case lookup..', 'ok');
      $docs = array();
      foreach ($data as $doc) {
        $docs[] = os2web_esdh_provider_invoke('cm', 'get_document', $doc);
      }
      error_log(print_r($docs, 1));
    }
  }
  else {
    drush_log('Api load', 'fail');
  }
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_acadre_cm_status() {
  if (os2web_esdh_provider_has_api('cm')) {
    drush_log('API load', 'ok');
    $avail = os2web_esdh_provider_invoke('cm', 'available');
    drush_log('CM availability check..', $avail ? 'ok' : 'error');
    $avail = os2web_esdh_provider_invoke('cm', 'available');
    drush_log('CM availability check..', $avail ? 'ok' : 'error');
  }
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_acadre_addenum_fix() {
  if (os2web_esdh_provider_has_api('mm')) {
    if (os2web_esdh_provider_supports('mm', 'post_import_process')) {
      os2web_esdh_provider_invoke('mm', 'post_import_process');
    }
  }
}

/**
 * Implements drush_COMMAND().
 */
function drush_os2web_acadre_esdh_delete_unused_dir($dir = NULL) {
  if (empty($dir) || !is_dir($dir)) {
    return drush_set_error('OS2WEB_ACADRE_ESDH_INVALID_DIR', dt('Invalid or missing directory argument.'));
  }

  $dry_run = drush_get_option('dry-run');
  $quiet   = drush_get_option('quiet') || drush_get_option('q');

  $unused = [];

  $dirs = scandir($dir);
  foreach ($dirs as $subdir) {
    if ($subdir === '.' || $subdir === '..') {
      continue;
    }

    $full_path = $dir . '/' . $subdir;
    if (!is_dir($full_path)) {
      continue;
    }

    // Only accept "Files_<int>_<int>".
    if (!preg_match('/^Files_\d+_\d+$/', $subdir)) {
      continue;
    }

    // Check if referenced in file_managed.
    $like = '%' . db_like($subdir) . '%';
    $count = db_query("SELECT COUNT(fid) FROM {file_managed} WHERE uri LIKE :like", [':like' => $like])->fetchField();

    if ($count == 0) {
      $unused[] = $full_path;
    }
  }

  if (!$dry_run) drush_print('Not dry run.');

  if (empty($unused)) {
    if (!$quiet) {
      drush_print('No unused directories found.');
    }
    return;
  }

  foreach ($unused as $path) {
    if (!$quiet) {
      drush_print($path);
    }
    if (!$dry_run) {
      drush_delete_dir($path, TRUE);
      if (!$quiet) {
        drush_print("Deleted " . $path);
      }
    }
  }
}
