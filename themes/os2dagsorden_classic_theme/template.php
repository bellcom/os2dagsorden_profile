<?php

/**
 * @file
 * Os2dagsorden_classic_theme.
 *
 * PHP version 5
 *
 * @category Themes
 * @package Themes_os2dagsorden_classic_theme
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License
 *
 * This file is empty by default because the base theme chain (Alpha & Omega)
 * provides all the basic functionality. However, in case you wish to customize
 * the output that Drupal generates through Alpha & Omega this file is a good
 * place to do so.
 *
 * Alpha comes with a neat solution for keeping this file as clean as possible
 * while the code for your subtheme grows. Please read the README.txt in the
 * /preprocess and /process subfolders for more information on this topic.
 */

/**
 * Implements hook_preprocess_page().
 *
 * Adds needed JS behaviour, loads the notes/speaker paper indicators, makes the
 * security log entries.
 */
function os2dagsorden_classic_theme_preprocess_page(&$variables) {
  drupal_add_library('system', 'ui.draggable');
  drupal_add_js(drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/js/jquery.ui.touch-punch.min.js');
  drupal_add_js(drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/js/os2dagsorden_classic_theme.js');

  if (!os2dagsorden_access_helper_is_touch()) {
    $collapse_menu = variable_get('os2dagsorden_collapse_menu', TRUE);
    drupal_add_js('add_show_hide_menu_behaviour(' . $collapse_menu . ');', 'inline');
  }
  else {
    $collapse_menu_touch = variable_get('os2dagsorden_collapse_menu_touch', TRUE);
    drupal_add_js('add_show_hide_menu_behaviour(' . $collapse_menu_touch . ');', 'inline');
  }
  drupal_add_js(array('os2dagsorden_settings' => array('body_font_size' => variable_get('os2dagsorden_body_text_size', '13'))), array('type' => 'setting'));
  drupal_add_js(array('os2dagsorden_settings' => array('title_font_size' => variable_get('os2dagsorden_title_text_size', '13'))), array('type' => 'setting'));
  drupal_add_js(array('os2dagsorden_settings' => array('sidepane_arrow_position' => variable_get('os2dagsorden_right_sidebar_arrow_position_radios', 'classic'))), array('type' => 'setting'));
  drupal_add_js(array('os2dagsorden_settings' => array('search_startdate' => date('d-m-Y', strtotime('-' . variable_get('os2dagsorden_search_startdate', 1) . ' months')))), array('type' => 'setting'));
  drupal_add_js(array('os2dagsorden_settings' => array('search_enddate' => date('d-m-Y', strtotime('+' . variable_get('os2dagsorden_search_enddate', 1) . ' months')))), array('type' => 'setting'));

  os2dagsorden_classic_theme_hide_menu_on_pages();

  drupal_add_js('add_indicator_help_text();', 'inline');
  if (variable_get('os2dagsorden_show_search_block_title', 'true') === 'false') {
    drupal_add_js('hide_search_block_title()', 'inline');
  }
  $view = views_get_page_view();
  if (!empty($view)) {
    global $base_path;
    if ($view->name === 'meeting_details') {
      $args =  arg();
      $meeting =  node_load($args[1]);
      // Adding expand/collapse behaviour to meeting details view.
      $os2dagsorden_expand_all_bullets = (variable_get('os2dagsorden_expand_all_bullets', FALSE)
          || (!empty($meeting->field_os2web_meetings_full_attac) && $meeting->field_os2web_meetings_full_attac['und'][0]['value'] ==1)) ?  1 : 0;
      $expand_attachment = variable_get('os2dagsorden_expand_attachment', TRUE);
      $expand_attachment_onload = (variable_get('os2dagsorden_expand_attachment_onload', FALSE)
          || (!empty($meeting->field_os2web_meetings_full_attac) && $meeting->field_os2web_meetings_full_attac['und'][0]['value'] ==1)) ? 1 : 0;
      drupal_add_js('bullet_point_add_expand_behaviour("' . $base_path . '?q=", ' . $expand_attachment . ',  ' . $os2dagsorden_expand_all_bullets . ' , ' . $expand_attachment_onload . ')', 'inline');
      if (module_exists('os2dagsorden_create_agenda') && user_access('administer os2web')) {
        drupal_add_js(drupal_get_path('module', 'os2dagsorden_create_agenda') . '/js/form_js.js');
      }
      $expand_bilags = variable_get('os2dagsorden_expand_bilags', "true");
      $expand_cases = variable_get('os2dagsorden_expand_cases', "false");
      drupal_add_js('open_all_bilag_case_bullet_points(' . $expand_bilags . ',' . $expand_cases . ')', 'inline');
      $variables['views'] = '';

      // Adding pagescroll.
      drupal_add_css(drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/css/pagescroller.skins.css');
      drupal_add_js(drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/js/jquery.pagescroller.js');
      drupal_add_js('addPagescroller();', 'inline');
    }
    if ($view->name === 'meeting_details' || $view->name === 'speaking_paper') {

      // Adding has notes indicator to attachment.
      $annotations = os2dagsorden_annotator_get_notes_by_meeting_id(arg(1));

      $attachment_ids = array();
      foreach ($annotations as $note) {
        $attachment_ids[] = $note->bilag_id;
      }
      $attachment_ids = array_unique($attachment_ids);
      $attachment_ids = implode(",", $attachment_ids);

      drupal_add_js('ids = [' . $attachment_ids . ']; bullet_point_attachment_add_notes_indicator(ids)', 'inline');
      // Reforcing the help text to be added.
      drupal_add_js('add_indicator_help_text();', 'inline');

      // Adding annotation.
      drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/annotator.min.js');
      //drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/touch-plugin/annotator.touch-syddjurs.min.js');
       drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/annotator-touch-plugin/annotator.touch.min.js');
      drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/imageselect-plugin/jquery.imgareaselect.min.js');
      drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/imageselect-plugin/annotator.imgselect.js');
       drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/json2.js');
      drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/XPath.js');
      drupal_add_css(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/annotator-full.min.css');
      drupal_add_css(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/annotator-touch-plugin/annotator.touch.css');
      drupal_add_css(drupal_get_path('module', 'os2dagsorden_annotator') . '/lib/imageselect-plugin/imgareaselect-default.css');

      $annotatorButtonText = variable_get('os2dagsorden_create_note_text', 'Lav note');
      $annotatorHideText = variable_get('os2dagsorden_create_note_hide_text', FALSE);
      $annotatorUseTitle = variable_get('os2dagsorden_create_note_text_use_title', FALSE);
      drupal_add_js(array(
          'os2dagsorden_annotator' =>
            array(
              'annotator_button_text' => $annotatorButtonText,
              'annotator_hide_text' => $annotatorHideText,
              'annotator_use_title' => $annotatorUseTitle
            )
        ),
        array('type' => 'setting'));

      drupal_add_js(drupal_get_path('module', 'os2dagsorden_annotator') . '/js/os2dagsorden_annotator_secure.js');
    }
    if ($view->name === 'speaking_paper') {
      // Adding expand/collapse behaviour bullet point details view.
      drupal_add_js('bullet_point_details_init("' . $base_path . '?q=", ' . variable_get('os2dagsorden_expand_attachment', TRUE) . ', ' . variable_get('os2dagsorden_expand_attachment_onload', 'false') . ')', 'inline');
      drupal_add_js('open_all_bilag_case_bullet_points(' . variable_get('os2dagsorden_expand_bilags', "true") . ',' . variable_get('os2dagsorden_expand_cases', "false") . ')', 'inline');

    }
    if (variable_get('os2dagsorden_show_massive_expand_collapse_button', 'true') === 'false' && ($view->name === 'speaking_paper' || $view->name === 'meeting_details')) {
      drupal_add_js('hide_massive_expand_collapse_button();', 'inline');
    }
  }
  else {
    if (isset($variables['page']['content']['content']['content']['system_main']) &&
      $variables['page']['content']['content']['content']['system_main']['content']['#attributes']['class'][1] === 'node-os2web_meetings_spaper-form') {
      // In "creating speaker paper"
      // hide extra fields.
      drupal_add_js("jQuery(document).ready(function(){jQuery('.form-item-field-os2web-meetings-sp-bullet-und-0-target-id').hide();});", "inline");

      // Setting breadcrumb.
      $destination = $_GET['destination'];
      $destination = explode('/', $destination);

      $breadcrumb = array();
      $breadcrumb[] = l(t('Forsiden'), $GLOBALS['base_url']);
      $breadcrumb[] .= l(t('Mødedetaljer'), 'meeting/' . $destination[1]);

      // Bullet point.
      if (isset($destination[3])) {
        $breadcrumb[] .= l(t('Dagsordenspunkt'), 'meeting/' . $destination[1] . '/bullet-point/' . $destination[3]);
      }

      $breadcrumb[] .= '<span class="breadcrumb-active">Opret talepapir</span>';
      drupal_set_breadcrumb($breadcrumb);
    }
  }
}

/**
 * Returns HTML for a breadcrumb trail.
 *
 * @param mixed $variables
 *   An associative array containing:
 *   - breadcrumb: An array containing the breadcrumb links.
 */
function os2dagsorden_classic_theme_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];

  if (!empty($breadcrumb)) {
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    if (count($breadcrumb) > 1) {
      $output .= '<div class="breadcrumb">' . implode(' » ', $breadcrumb) . '</div>';
    }

    return $output;
  }
}

/**
 * Implementation of theming the calendar title.
 *
 * Change the format of navigation title in calendar day view to be [weekday],
 * [day]. [month] [year].
 *
 * @param mixed $params
 *   Params.
 *
 * @return string
 *   Reformatted title.
 */
function os2dagsorden_classic_theme_date_nav_title($params) {
  $granularity = $params['granularity'];
  $view = $params['view'];
  $date_info = $view->date_info;
  $link = !empty($params['link']) ? $params['link'] : FALSE;
  $format = !empty($params['format']) ? $params['format'] : NULL;

  $title = '';
  $date_arg = '';

  switch ($granularity) {
    case 'year':
      $title = $date_info->year;
      $date_arg = $date_info->year;
      break;

    case 'month':
      $format = !empty($format) ? $format : (empty($date_info->mini) ? 'F Y' : 'F Y');
      $title = date_format_date($date_info->min_date, 'custom', $format);
      $date_arg = $date_info->year . '-' . date_pad($date_info->month);
      break;

    case 'day':
      $format = !empty($format) ? $format : (empty($date_info->mini) ? 'l, j. F Y' : 'l, F j');
      $title = date_format_date($date_info->min_date, 'custom', $format);
      $date_arg = $date_info->year . '-' . date_pad($date_info->month) . '-' . date_pad($date_info->day);
      break;

    case 'week':
      $format = !empty($format) ? $format : (empty($date_info->mini) ? 'F j, Y' : 'F j');
      $title = t('Week of @date', array(
        '@date' => date_format_date($date_info->min_date, 'custom', $format),
      ));
      $date_arg = $date_info->year . '-W' . date_pad($date_info->week);
      break;
  }
  if (!empty($date_info->mini) || $link) {
    // Month navigation titles are used as links in the mini view.
    $attributes = array('title' => t('View full page month'));
    $url = date_pager_url($view, $granularity, $date_arg, TRUE);
    return l($title, $url, array('attributes' => $attributes));
  }
  else {
    return $title;
  }
}

/**
 * Format the time row headings in the week and day view.
 *
 * Change the time format to be [hour].[minutes].
 *
 * @param mixed $vars
 *   Vars.
 *
 * @return string
 *   Reformatted title.
 */
function os2dagsorden_classic_theme_calendar_time_row_heading($vars) {
  $start_time = $vars['start_time'];
  $next_start_time = $vars['next_start_time'];
  $curday_date = $vars['curday_date'];
  static $format_hour, $format_ampm;
  if (empty($format_hour)) {
    $format = variable_get('date_format_short', 'm/d/Y - H:i');
    $limit = array('hour', 'minute');
    $format_hour = str_replace(array('a', 'A'), '', date_limit_format($format, $limit));
    $format_ampm = strstr($format, 'a') ? 'a' : (strstr($format, 'A') ? 'A' : '');
  }
  if ($start_time === '00:00:00' && $next_start_time === '23:59:59') {
    $hour = t('All times');
  }
  elseif ($start_time === '00:00:00') {
    $date = date_create($curday_date . ' ' . $next_start_time);
    $hour = t('Before @time', array('@time' => date_format($date, $format_hour)));
  }
  else {
    $date = date_create($curday_date . ' ' . $start_time);
    $hour = date_format($date, $format_hour);
  }
  if (!empty($date)) {
    $ampm = date_format($date, $format_ampm);
  }
  else {
    $ampm = '';
  }
  return array('hour' => $hour, 'ampm' => $ampm);
}

/**
 * Changes the format of the exposed form - meetings search.
 *
 * Also removes the unneeded links on log in page.
 *
 * @param mixed $form
 *   Form.
 * @param mixed $form_state
 *   Form state.
 */
function os2dagsorden_classic_theme_form_alter(&$form, &$form_state) {
  if ($form['#id'] === 'views-exposed-form-meetings-search-page') {
    $form['from_date']['value']['#date_format'] = 'd-m-Y';
    $form['to_date']['value']['#date_format'] = 'd-m-Y';
    $args = arg();
    if (variable_get('os2dagsorden_search_restrict_search_function', FALSE) && $args[0] === 'meeting' && isset($args[1])) {
      $meeting =  node_load($args[1]);
      if ($meeting && $meeting->type == 'os2web_meetings_meeting') {
        $form_state['view']->args = array($meeting->nid);
        $old_value = date_create_from_format("Y-m-d H:i:s", $meeting->field_os2web_meetings_date['und'][0]['value']);
        $old_value = $old_value->format('d-m-Y');
        $form_state['input']['from_date']['value']['date'] = $old_value;
        $form_state['input']['to_date']['value']['date'] = $old_value;
        $available_commitees = array_keys($form['field_os2web_meetings_committee_tid_depth']['#options']);
        if (in_array($meeting->field_os2web_meetings_committee['und'][0]['tid'], $available_commitees)) {
          $form_state['input']['field_os2web_meetings_committee_tid_depth'] = $meeting->field_os2web_meetings_committee['und'][0]['tid'];
        }
      }
    }
    else {
    if (isset($_SESSION['views']) && !is_array($_SESSION['views']['meetings_search']['page']['from_date']['value'])) {
      if (!empty($_SESSION['views']['meetings_search']['page']['from_date']['value'])) {
        $old_value = $_SESSION['views']['meetings_search']['page']['from_date']['value'];
        $_SESSION['views']['meetings_search']['page']['from_date']['value'] = array();
        $old_value = date_create_from_format("Y-m-d", $old_value);
        $old_value = $old_value->format('d-m-Y');
        $_SESSION['views']['meetings_search']['page']['from_date']['value']['date'] = $old_value;
      }
    }

    if (isset($_SESSION['views']) && !is_array($_SESSION['views']['meetings_search']['page']['to_date']['value'])) {
      if (!empty($_SESSION['views']['meetings_search']['page']['to_date']['value'])) {
        $old_value = $_SESSION['views']['meetings_search']['page']['to_date']['value'];
        $_SESSION['views']['meetings_search']['page']['to_date']['value'] = array();
        $old_value = date_create_from_format("Y-m-d", $old_value);
        $old_value = $old_value->format('d-m-Y');
        $_SESSION['views']['meetings_search']['page']['to_date']['value']['date'] = $old_value;
      }
    }
    }
  }
  elseif ($form['#id'] === 'user-login-form') {
    $form['name']['#description'] = "";
    $form['pass']['#description'] = "";
    $form['links']['#markup'] = "";
  }
  elseif ($form['#id'] === 'views-exposed-form-meetings-search-page-meetings-overview') {
    $form['from_date']['value']['#date_format'] = 'd-m-Y';
    $form['to_date']['value']['#date_format'] = 'd-m-Y';
  }
}

/**
 * Preprocess HTML hook.
 *
 * Fixes the IE compatibility problem.
 *
 * @param mixed $vars
 *   Array of variables.
 */
function os2dagsorden_classic_theme_preprocess_html(&$vars) {
  // Setup IE meta tag to force IE rendering mode.
  $meta_ie_render_engine = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
      'http-equiv' => 'X-UA-Compatible',
      'content' => 'IE=8,IE=Edge,chrome=1',
    ),
    '#weight' => '-99999',
  );

  $format_detection = array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
      'name' => 'format-detection',
      'content' => 'telephone=no',
    ),
    '#weight' => '-99999',
  );

  // <meta name="format-detection" content="telephone=no">
  // Add header meta tag for IE to head.
  drupal_add_html_head($meta_ie_render_engine, 'meta_ie_render_engine');
  drupal_add_html_head($format_detection, 'format-detection');
}

/**
 * Implements template_theme_menu_local_tasks.
 *
 * @param mixed $variables
 *   Variables.
 *
 * @return string
 *   Themed local tasks.
 */
function os2dagsorden_classic_theme_menu_local_task($variables) {
  $link = $variables['element']['#link'];
  $href = explode('/', $link['href']);
  $node = node_load($href[1]);

  // Disabling edit tab, if only node type is not page.
  if ($link['path'] === 'node/%/edit' && $node->type !== 'os2web_page') {
    return '';
  }
  else {
    // Disabling view tab.
    if ($link['path'] === 'node/%/view') {
      return '';
    }
    else {
      if ($link['path'] === 'user/%/edit' || $link['path'] === 'user/%/view' || $link['path'] === 'user/%/simple_edit') {
        return '';
      }
      else {
        return theme_menu_local_task($variables);
      }
    }
  }
}

/**
 * Hides the menu on some pages.
 */
function os2dagsorden_classic_theme_hide_menu_on_pages() {
  if (os2dagsorden_access_helper_is_touch()) {
    $pages = variable_get('os2dagsorden_collapse_menu_touch_pages', 'meetings/print');
    $pages = explode(PHP_EOL, $pages);
    foreach ($pages as $page) {
      if (strcmp($page, current_path()) === 0) {
        drupal_add_js('hide_side_menu(false);', 'inline');
        return;
      }
    }
    // No pages forcing menu to be hidden, can add orientation listener.
    drupal_add_js('add_tablet_orientation_listener();', 'inline');
  }
}

/**
 * Implements hook_preprocess_node().
 */
function os2dagsorden_classic_theme_preprocess_node(&$variables) {
  // Add css class "node--NODETYPE--VIEWMODE" to nodes.
  $variables['classes_array'][] = 'node--' . $variables['type'] . '--' . $variables['view_mode'];

  // Make "node--NODETYPE--VIEWMODE.tpl.php" templates available for nodes.
  $variables['theme_hook_suggestions'][] = 'node__' . $variables['type'] . '__' . $variables['view_mode'];
}
