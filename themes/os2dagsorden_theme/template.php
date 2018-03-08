<?php

/**
 * @file
 * Hook preprocess page.
 */

/**
 * Implement template_preprocess_page.
 *
 * @param mixed $variables
 *   Variables.
 */
function os2dagsorden_theme_preprocess_page(&$variables) {
  // Color.
  if (module_exists('color')) {
    _color_page_alter($variables);
  }
}

/**
 * Implements template_preprocess_html.
 *
 * @param mixed $variables
 *   Variables.
 */
function os2dagsorden_theme_process_html(&$variables) {
  // Hook into color.module.
  if (module_exists('color')) {
    _color_html_alter($variables);
  }
}
