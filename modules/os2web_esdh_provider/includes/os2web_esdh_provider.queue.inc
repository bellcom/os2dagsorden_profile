<?php

/**
 * @file
 * os2web_esdh_provider.queue.inc
 */

const IMPORT_STATUS_SCHEDULED = "Scheduled";
const IMPORT_STATUS_IMPORTED = "Imported";
const IMPORT_STATUS_IGNORED = "Ignored";
const IMPORT_STATUS_DELETED = "Deleted";

/**
 * This function adds a meeting entry to the database.
 *
 * @param int $nid
 *   Meeting id.
 * @param mixed $manifest
 *   Manifest array.
 * @param mixed $agenda
 *   Agenda array.
 */
function os2web_esdh_provider_index_meeting($nid, $manifest, $agenda) {
  $status = IMPORT_STATUS_SCHEDULED;
  if ($nid) {
    $status = IMPORT_STATUS_IMPORTED;
  }
  if (empty($agenda['publish']) || $agenda['publish'] == 0) {
    $status = IMPORT_STATUS_IGNORED;
  }

  $manifest_real_path = drupal_realpath($manifest['uri']);
  $agenda_real_path = drupal_realpath($agenda['path']);

  // Trying to update existing entry.
  if (os2web_esdh_provider_index_meeting_is_meeting_indexed($manifest['id'], $agenda['meeting_sub_id'])) {
    db_update('os2web_esdh_provider_import')
      ->fields(array(
        'nid' => $nid ? $nid : 0,
        'import_status' => $status,
        'meeting_xml' => $manifest['uri'],
        'meeting_name' => $agenda['title'],
        'meeting_xml_changed' => ($manifest_real_path !== FALSE) ? filemtime($manifest_real_path) : NULL,
        'agenda_processed' => $_SERVER['REQUEST_TIME'],
        'agenda_xml' => $agenda['agenda_xml'],
        'agenda_xmlid' => $agenda['agenda_xmlid'],
        'agenda_access' => ($agenda['agenda_access'] === 'Lukket') ? 0 : 1,
        'agenda_publish' => ($agenda['publish']) ? 1 : 0,
        'agenda_type' => $agenda['type'],
        'agenda_xml_changed' => ($agenda_real_path !== FALSE) ? filemtime($agenda_real_path) : NULL,
        'committeed_id' => $agenda['committee_id'],
      ))
      ->condition('meeting_xmlid', $manifest['id'], '=')
      ->condition('agenda_sysid', $agenda['meeting_sub_id'], '=')
      ->execute();
    // No entry found - create a new entry.
  }
  else {
    db_insert('os2web_esdh_provider_import')
      ->fields(array(
        'nid' => $nid ? $nid : 0,
        'import_status' => $status,
        'meeting_xml' => $manifest['uri'],
        'meeting_xmlid' => $manifest['id'],
        'meeting_name' => $agenda['title'],
        'meeting_sysid' => $agenda['meeting_id'],
        'meeting_xml_changed' => ($manifest_real_path !== FALSE) ? filemtime($manifest_real_path) : NULL,
        'agenda_discovered' => $_SERVER['REQUEST_TIME'],
        'agenda_sysid' => $agenda['meeting_sub_id'],
        'agenda_xml' => $agenda['agenda_xml'],
        'agenda_xmlid' => $agenda['agenda_xmlid'],
        'agenda_access' => ($agenda['agenda_access'] === 'Lukket') ? 0 : 1,
        'agenda_publish' => ($agenda['publish']) ? 1 : 0,
        'agenda_type' => $agenda['type'],
        'agenda_xml_changed' => ($agenda_real_path !== FALSE) ? filemtime($agenda_real_path) : NULL,
        'committeed_id' => $agenda['committee_id'],
        'committeed_tid' => 0,
        'plugin_id' => $manifest['plugin_id'],
      ))
      ->execute();
  }
}

/**
 * Check if manifest is outdated.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 * @param int $currentXmlChanged
 *   XML file modify time.
 *
 * @return bool
 *   TRUE if manifest is outdated, otherwise - FALSE.
 */
function os2web_esdh_provider_index_meeting_is_manifest_outdated($meeting_xmlid, $currentXmlChanged) {
  // Manifest is not indexed.
  if (!os2web_esdh_provider_index_meeting_is_meeting_indexed($meeting_xmlid)) {
    return TRUE;
  }

  $rowCount = db_select('os2web_esdh_provider_import', 'o')
    ->fields('o')
    ->condition('meeting_xmlid', $meeting_xmlid, '=')
    ->condition('meeting_xml_changed', $currentXmlChanged, '<')
    ->execute()
    ->rowCount();

  return $rowCount > 0;
}

/**
 * Check, if meeting has deleted agendas.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 *
 * @return bool
 *   TRUE if meeting has deleted agendas, otherwise - FALSE.
 */
function os2web_esdh_provider_index_meeting_has_deleted_agendas($meeting_xmlid) {
  $rowCount = db_select('os2web_esdh_provider_import', 'o')
    ->fields('o')
    ->condition('meeting_xmlid', $meeting_xmlid, '=')
    ->condition('import_status', IMPORT_STATUS_DELETED, '=')
    ->execute()
    ->rowCount();

  return $rowCount > 0;
}

/**
 * Check, if meeting is indexed.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 * @param int|bool $agenda_sysid
 *   Agenda sysid.
 *
 * @return bool
 *   TRUE if meeting already indexed, otherwise - FALSE.
 */
function os2web_esdh_provider_index_meeting_is_meeting_indexed($meeting_xmlid, $agenda_sysid = FALSE) {
  $query = db_select('os2web_esdh_provider_import', 'o')
    ->fields('o')
    ->condition('meeting_xmlid', $meeting_xmlid, '=');

  if ($agenda_sysid) {
    $query->condition('agenda_sysid', $agenda_sysid, '=');
  }

  $rowCount = $query->execute()
    ->rowCount();

  return $rowCount > 0;
}

/**
 * This function updated a status of meeting entry in the database.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 * @param int|bool $agenda_sysid
 *   Agenda sysid.
 * @param string $status
 *   Status.
 */
function os2web_esdh_provider_index_meeting_update_status($meeting_xmlid, $agenda_sysid, $status) {
  $query = db_update('os2web_esdh_provider_import')
    ->fields(array(
      'import_status' => $status,
      'agenda_processed' => $_SERVER['REQUEST_TIME'],
    ))
    ->condition('meeting_xmlid', $meeting_xmlid, '=');

  if ($agenda_sysid) {
    $query->condition('agenda_sysid', $agenda_sysid, '=');
  }

  $query->execute();
}

/**
 * This function updated a nid of meeting entry in the database.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 * @param int|bool $agenda_sysid
 *   Agenda sysid.
 * @param int $nid
 *   Meeting ID.
 */
function os2web_esdh_provider_index_meeting_update_nid($meeting_xmlid, $agenda_sysid, $nid) {
  $query = db_update('os2web_esdh_provider_import')
    ->fields(array(
      'nid' => $nid,
      'agenda_processed' => $_SERVER['REQUEST_TIME'],
    ))
    ->condition('meeting_xmlid', $meeting_xmlid, '=');

  if ($agenda_sysid) {
    $query->condition('agenda_sysid', $agenda_sysid, '=');
  }

  $query->execute();
}

/**
 * This function get a nid of meeting entry by meeting xmlid.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 *
 * @return int
 *   Meeting ID.
 */
function os2web_esdh_provider_index_meeting_get_nid($meeting_xmlid) {
  $nid = db_select('os2web_esdh_provider_import', 'o')
    ->fields('o', array('nid'))
    ->condition('meeting_xmlid', $meeting_xmlid, '=')
    ->condition('import_status', IMPORT_STATUS_IMPORTED, '=')
    ->execute()
    ->fetchField();

  return $nid;
}

/**
 * This function updated a committee_id of meeting entry in the database.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 * @param int|bool $agenda_sysid
 *   Agenda sysid.
 * @param int $tid
 *   Committee ID.
 */
function os2web_esdh_provider_index_meeting_update_committee_tid($meeting_xmlid, $agenda_sysid, $tid) {
  db_update('os2web_esdh_provider_import')
    ->fields(array(
      'committeed_tid' => $tid,
    ))
    ->condition('meeting_xmlid', $meeting_xmlid, '=')
    ->condition('agenda_sysid', $agenda_sysid, '=')
    ->execute();
}

/**
 * Check, if meeting has all agendas as ignored.
 *
 * @param int $meeting_xmlid
 *   Meeting XML ID.
 *
 * @return bool
 *   TRUE if all meeting agendas are ignored, otherwise - FALSE.
 */
  function os2web_esdh_provider_index_meeting_is_ignored($meeting_xmlid) {
  $import_status = db_select('os2web_esdh_provider_import', 'o')
    ->distinct()
    ->fields('o', array('import_status'))
    ->condition('meeting_xmlid', $meeting_xmlid, '=')
    ->execute()
    ->fetchCol();

  if (count($import_status) == 1 && in_array(IMPORT_STATUS_IGNORED, $import_status)) {
    return TRUE;
  }

  return FALSE;
}
