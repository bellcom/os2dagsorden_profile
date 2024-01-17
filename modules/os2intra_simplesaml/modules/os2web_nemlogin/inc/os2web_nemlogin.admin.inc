<?php

/**
 * Audit trail configuration form.
 */
function os2web_nemlogin_settings_form($form, $form_state) {
  $form = array();

  $form['os2web_nemlogin_idp_base_url'] = [
    '#type' => 'textfield',
    '#title' => t('Base URL of IDP system.'),
    '#required' => TRUE,
    '#default_value' => variable_get('os2web_nemlogin_idp_base_url'),
    '#description' => t('E.g. https://nemlogin3.bellcom.dk/simplesaml. NB! Do not include the trailing slash.'),
  ];
  $form['os2web_nemlogin_mnemo'] = [
    '#type' => 'textfield',
    '#title' => t('IDP mnemo key.'),
    '#required' => TRUE,
    '#default_value' => variable_get('os2web_nemlogin_mnemo'),
    '#description' => t('Value for IDP mnemo key. Example: bellcom.dk'),
  ];
  $form['os2web_nemlogin_uid_field'] = [
    '#type' => 'select',
    '#title' => t('User identifier field (uid)'),
    '#options' => ['pid' => 'PID', 'cpr_uuid' => 'CPR Uuid', 'cpr' => 'CPR (encrypted)'],
    '#required' => TRUE,
    '#default_value' => variable_get('os2web_nemlogin_uid_field'),
    '#description' => t('Field for identifying the user, must be available in the session'),
  ];
  $form['os2web_nemlogin_enable_select_login_page'] = [
    '#type' => 'checkbox',
    '#title' => t('Enabled select login page'),
    '#default_value' => variable_get('os2web_nemlogin_enable_select_login_page'),
    '#description' => t('If enabled user will be redirected to select login type page - SimpleSAML and Nemlogin. Otherwise - will be redirected to Nemlogin page directly'),
  ];

  return system_settings_form($form);
}


function os2web_nemlogin_settings_form_validate() {
  // TODO:
}
