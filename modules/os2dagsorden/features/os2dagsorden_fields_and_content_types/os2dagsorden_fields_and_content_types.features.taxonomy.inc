<?php
/**
 * @file
 * os2dagsorden_fields_and_content_types.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function os2dagsorden_fields_and_content_types_taxonomy_default_vocabularies() {
  return array(
    'os2web_meetings_tax_committee_category' => array(
      'name' => 'Committee category',
      'machine_name' => 'os2web_meetings_tax_committee_category',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
    'os2web_users_tax_sp_group' => array(
      'name' => 'Speaker paper user groups',
      'machine_name' => 'os2web_users_tax_sp_group',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
    ),
  );
}
