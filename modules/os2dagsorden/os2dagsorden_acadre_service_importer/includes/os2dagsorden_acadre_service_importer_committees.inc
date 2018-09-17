<?php

/**
 * @file
 * OS2Dagsorden_acadre_service_imported_committees.inc.
 */

/**
 * Helper function.
 *
 * Processes the committees xml. Delegates to create committees.
 *
 * @param SimpleXMLElement $committees_rows
 *   Array of committees.
 */
function _os2dagsorden_acadre_service_importer_process_committees(SimpleXMLElement $committees_rows) {
  $vid = db_select('taxonomy_vocabulary', 'tv')
    ->fields('tv', array('vid'))
    ->condition('machine_name', 'os2web_meetings_tax_committee')
    ->execute()
    ->fetchField();

  // Marking all committees as disabled, they will be enabled by the loop below.
  $terms = taxonomy_get_tree($vid);
  foreach ($terms as $term) {
    $term->field_os2web_meetings_com_active['und'][0]['value'] = 0;
    taxonomy_term_save($term);
  }

  foreach ($committees_rows as $row) {
    $xml_data = array(
      'sub_id' => (string) $row->Id,
      'committee_name' => (string) $row->Name,
      'state' => (string) $row->State,
      'active' => (string) $row->Active,
    );

    // Create/update committee if missing.
    os2dagsorden_acadre_service_importer_create_committee($xml_data, $vid);
  }
}

/**
 * Helper function. Creates/updates the committee.
 *
 * @param array $xml_data
 *   Xml data.
 *
 * @return int
 *   The id of created/updated entry.
 */
function os2dagsorden_acadre_service_importer_create_committee(array $xml_data, $vid) {
  // Getting committee by sub ID.
  $term = _os2dagsorden_access_helper_get_committee_by_subid_and_name($xml_data['sub_id'], $xml_data['committee_name']);

  // No success, creating new committee and setting sub id.
  if (!$term) {
    $term = new stdClass();
    $term->vid = $vid;
    $term->field_os2web_meetings_com_subid['und'][]['value'] = $xml_data['sub_id'];
    $term->name = $xml_data['committee_name'];
  }

  if (!isset($term->field_os2web_meetings_com_shname['und']) || empty($term->field_os2web_meetings_com_shname['und'][0]['value'])) {
    $term->field_os2web_meetings_com_shname['und'][0]['value'] = $xml_data['committee_name'];
  }
  if (!isset($term->field_os2web_meetings_com_abbr['und']) || empty($term->field_os2web_meetings_com_abbr['und'][0]['value'])) {
    $term->field_os2web_meetings_com_abbr['und'][0]['value'] = $xml_data['committee_name'];
  }
  if (!isset($term->field_os2web_meetings_com_categ['und']) || empty($term->field_os2web_meetings_com_categ['und'][0]['tid'])) {
    $cat_voc = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_committee_category');
    $cat_terms = taxonomy_get_tree($cat_voc->vid);

    if (empty($cat_terms)) {
      $term->field_os2web_meetings_com_categ['und'][0]['tid'] = $cat_terms[0]->tid;
    }
  }

  // See if we need to enable term or its parent.
  if ($xml_data['state'] === 'Aktivt' && $xml_data['active'] === 'true') {
    $parents = taxonomy_get_parents($term->tid);
    if (empty($parents)) {
      $term->field_os2web_meetings_com_active['und'][0]['value'] = 1;
    }
    else {
      foreach ($parents as $parent) {
        $parent->field_os2web_meetings_com_active['und'][0]['value'] = 1;
        taxonomy_term_save($parent);
      }
    }
  }

  taxonomy_term_save($term);
  return $term->tid;
}
