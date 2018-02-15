<?php

/**
 * @file
 * This file describes the admin form for os2web.
 */

/**
 * Admin settings forms.
 *
 * @param mixed $form
 *   Form.
 * @param mixed $form_state
 *   Form state.
 *
 * @return mixed
 *   System setting form.
 */
function os2web_settings_settings_form($form, $form_state) {
  $form['dummy'] = array(
    '#type' => 'markup',
    '#value' => 'Placeholder until more settings are needed.',
  );

  return system_settings_form($form);
}
