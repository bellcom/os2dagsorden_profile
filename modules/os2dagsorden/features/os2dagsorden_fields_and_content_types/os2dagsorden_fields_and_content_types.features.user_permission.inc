<?php
/**
 * @file
 * os2dagsorden_fields_and_content_types.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function os2dagsorden_fields_and_content_types_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'create field_follows_committee_plus'.
  $permissions['create field_follows_committee_plus'] = array(
    'name' => 'create field_follows_committee_plus',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_follows_committee_pp'.
  $permissions['create field_follows_committee_pp'] = array(
    'name' => 'create field_follows_committee_pp',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_can_send_closed_bp'.
  $permissions['create field_user_can_send_closed_bp'] = array(
    'name' => 'create field_user_can_send_closed_bp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_committee'.
  $permissions['create field_user_committee'] = array(
    'name' => 'create field_user_committee',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_forrbidden_categories'.
  $permissions['create field_user_forrbidden_categories'] = array(
    'name' => 'create field_user_forrbidden_categories',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_id'.
  $permissions['create field_user_id'] = array(
    'name' => 'create field_user_id',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_login_token'.
  $permissions['create field_user_login_token'] = array(
    'name' => 'create field_user_login_token',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_premeeting_committee'.
  $permissions['create field_user_premeeting_committee'] = array(
    'name' => 'create field_user_premeeting_committee',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'create field_user_sp_groups'.
  $permissions['create field_user_sp_groups'] = array(
    'name' => 'create field_user_sp_groups',
    'roles' => array(
      'anonymous user' => 'anonymous user',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_follows_committee_plus'.
  $permissions['edit field_follows_committee_plus'] = array(
    'name' => 'edit field_follows_committee_plus',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_follows_committee_pp'.
  $permissions['edit field_follows_committee_pp'] = array(
    'name' => 'edit field_follows_committee_pp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_can_send_closed_bp'.
  $permissions['edit field_user_can_send_closed_bp'] = array(
    'name' => 'edit field_user_can_send_closed_bp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_committee'.
  $permissions['edit field_user_committee'] = array(
    'name' => 'edit field_user_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_forrbidden_categories'.
  $permissions['edit field_user_forrbidden_categories'] = array(
    'name' => 'edit field_user_forrbidden_categories',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_id'.
  $permissions['edit field_user_id'] = array(
    'name' => 'edit field_user_id',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_login_token'.
  $permissions['edit field_user_login_token'] = array(
    'name' => 'edit field_user_login_token',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_premeeting_committee'.
  $permissions['edit field_user_premeeting_committee'] = array(
    'name' => 'edit field_user_premeeting_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit field_user_sp_groups'.
  $permissions['edit field_user_sp_groups'] = array(
    'name' => 'edit field_user_sp_groups',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_follows_committee_plus'.
  $permissions['edit own field_follows_committee_plus'] = array(
    'name' => 'edit own field_follows_committee_plus',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_follows_committee_pp'.
  $permissions['edit own field_follows_committee_pp'] = array(
    'name' => 'edit own field_follows_committee_pp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_can_send_closed_bp'.
  $permissions['edit own field_user_can_send_closed_bp'] = array(
    'name' => 'edit own field_user_can_send_closed_bp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_committee'.
  $permissions['edit own field_user_committee'] = array(
    'name' => 'edit own field_user_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_forrbidden_categories'.
  $permissions['edit own field_user_forrbidden_categories'] = array(
    'name' => 'edit own field_user_forrbidden_categories',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_id'.
  $permissions['edit own field_user_id'] = array(
    'name' => 'edit own field_user_id',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_login_token'.
  $permissions['edit own field_user_login_token'] = array(
    'name' => 'edit own field_user_login_token',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_premeeting_committee'.
  $permissions['edit own field_user_premeeting_committee'] = array(
    'name' => 'edit own field_user_premeeting_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'edit own field_user_sp_groups'.
  $permissions['edit own field_user_sp_groups'] = array(
    'name' => 'edit own field_user_sp_groups',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_follows_committee_plus'.
  $permissions['view field_follows_committee_plus'] = array(
    'name' => 'view field_follows_committee_plus',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_follows_committee_pp'.
  $permissions['view field_follows_committee_pp'] = array(
    'name' => 'view field_follows_committee_pp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_can_send_closed_bp'.
  $permissions['view field_user_can_send_closed_bp'] = array(
    'name' => 'view field_user_can_send_closed_bp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_committee'.
  $permissions['view field_user_committee'] = array(
    'name' => 'view field_user_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_forrbidden_categories'.
  $permissions['view field_user_forrbidden_categories'] = array(
    'name' => 'view field_user_forrbidden_categories',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_id'.
  $permissions['view field_user_id'] = array(
    'name' => 'view field_user_id',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_login_token'.
  $permissions['view field_user_login_token'] = array(
    'name' => 'view field_user_login_token',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_premeeting_committee'.
  $permissions['view field_user_premeeting_committee'] = array(
    'name' => 'view field_user_premeeting_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view field_user_sp_groups'.
  $permissions['view field_user_sp_groups'] = array(
    'name' => 'view field_user_sp_groups',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_follows_committee_plus'.
  $permissions['view own field_follows_committee_plus'] = array(
    'name' => 'view own field_follows_committee_plus',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_follows_committee_pp'.
  $permissions['view own field_follows_committee_pp'] = array(
    'name' => 'view own field_follows_committee_pp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_can_send_closed_bp'.
  $permissions['view own field_user_can_send_closed_bp'] = array(
    'name' => 'view own field_user_can_send_closed_bp',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_committee'.
  $permissions['view own field_user_committee'] = array(
    'name' => 'view own field_user_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_forrbidden_categories'.
  $permissions['view own field_user_forrbidden_categories'] = array(
    'name' => 'view own field_user_forrbidden_categories',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_id'.
  $permissions['view own field_user_id'] = array(
    'name' => 'view own field_user_id',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_login_token'.
  $permissions['view own field_user_login_token'] = array(
    'name' => 'view own field_user_login_token',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_premeeting_committee'.
  $permissions['view own field_user_premeeting_committee'] = array(
    'name' => 'view own field_user_premeeting_committee',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  // Exported permission: 'view own field_user_sp_groups'.
  $permissions['view own field_user_sp_groups'] = array(
    'name' => 'view own field_user_sp_groups',
    'roles' => array(
      'administrator+' => 'administrator+',
      'administrator++' => 'administrator++',
      'os2dagsorden' => 'os2dagsorden',
    ),
    'module' => 'field_permissions',
  );

  return $permissions;
}
