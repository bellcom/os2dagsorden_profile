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

  return system_settings_form($form);
}


function os2web_nemlogin_settings_form_validate() {
  // TODO:
}
