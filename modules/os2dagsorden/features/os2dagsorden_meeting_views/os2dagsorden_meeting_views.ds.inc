<?php

/**
 * @file
 * os2dagsorden_meeting_views.ds.inc
 */

/**
 * Implements hook_ds_field_settings_info().
 */
function os2dagsorden_meeting_views_ds_field_settings_info() {
  $export = array();

  $ds_fieldsetting = new stdClass();
  $ds_fieldsetting->api_version = 1;
  $ds_fieldsetting->id = 'user|user|os2dagsorden_users_select';
  $ds_fieldsetting->entity_type = 'user';
  $ds_fieldsetting->bundle = 'user';
  $ds_fieldsetting->view_mode = 'os2dagsorden_users_select';
  $ds_fieldsetting->settings = array(
    'name' => array(
      'format' => 'default',
      'formatter_settings' => array(
        'class' => '',
        'link' => '0',
        'wrapper' => '',
      ),
      'label' => 'hidden',
      'weight' => '1',
    ),
  );
  $export['user|user|os2dagsorden_users_select'] = $ds_fieldsetting;

  return $export;
}

/**
 * Implements hook_ds_layout_settings_info().
 */
function os2dagsorden_meeting_views_ds_layout_settings_info() {
  $export = array();

  $ds_layout = new stdClass();
  $ds_layout->api_version = 1;
  $ds_layout->id = 'user|user|os2dagsorden_users_select';
  $ds_layout->entity_type = 'user';
  $ds_layout->bundle = 'user';
  $ds_layout->view_mode = 'os2dagsorden_users_select';
  $ds_layout->layout = 'ds_1col';
  $ds_layout->settings = array(
    'classes' => array(),
    'fields' => array(
      'field_user_full_name' => 'ds_content',
      'name' => 'ds_content',
    ),
    'layout_attributes' => '',
    'layout_attributes_merge' => 1,
    'layout_disable_css' => FALSE,
    'layout_link_attribute' => '',
    'layout_link_custom' => '',
    'layout_wrapper' => 'span',
    'regions' => array(
      'ds_content' => array(
        0 => 'field_user_full_name',
        1 => 'name',
      ),
    ),
    'wrappers' => array(
      'ds_content' => 'span',
    ),
  );
  $export['user|user|os2dagsorden_users_select'] = $ds_layout;

  return $export;
}

/**
 * Implements hook_ds_view_modes_info().
 */
function os2dagsorden_meeting_views_ds_view_modes_info() {
  $export = array();

  $ds_view_mode = new stdClass();
  $ds_view_mode->api_version = 1;
  $ds_view_mode->view_mode = 'os2dagsorden_users_select';
  $ds_view_mode->label = 'OS2Dagsorden users select';
  $ds_view_mode->entities = array(
    'comment' => 'comment',
    'node' => 'node',
    'rules_config' => 'rules_config',
    'taxonomy_term' => 'taxonomy_term',
    'user' => 'user',
  );
  $export['os2dagsorden_users_select'] = $ds_view_mode;

  return $export;
}
