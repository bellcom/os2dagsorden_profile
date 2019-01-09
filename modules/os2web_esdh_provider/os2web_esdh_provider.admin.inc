<?php

/**
 * @file
 * Implements administration menus and functionality for os2web_esdh_provider.
 */

/**
 * Page callback. Displays status page.
 */
function _os2web_esdh_provider_status() {
  ctools_include('plugins');
  $apis = array_keys(os2web_esdh_provider_ctools_plugin_type());
  $head = array('API name', 'Description', 'module', 'status');
  $rows = array();
  foreach ($apis as $api) {
    $plugins = ctools_get_plugins('os2web_esdh_provider', $api);
    if (count($plugins) > 0) {
      foreach ($plugins as $plugin) {
        $rows[] = array(
          $api,
          $plugin['description'],
          $plugin['module'],
          os2web_esdh_provider_has_api($api) ? 'Working' : 'Error',
        );
      }
    }
    else {
      $rows[] = array($api, 'Unavailable', 'Not enabled.', 'Error');
    }
  }

  // Show a table of watchdog entries if the debug flag is on.
  $watchdog_table = '';
  if (variable_get('os2web_esdh_provider_debug_info')) {
    $watchdog_rows = array();
    $watchdog_messages = db_query('SELECT * FROM {watchdog} w WHERE w.type = :type AND w.severity = :severity ORDER BY w.timestamp DESC', array(
      ':type' => 'acadre MM',
      ':severity' => WATCHDOG_WARNING,
    ));
    $watchdog_head = array(t('Warning messages in descending order'), t('Date'));
    foreach ($watchdog_messages as $row) {
      $watchdog_rows[] = array(
        format_string($row->message, unserialize($row->variables)),
        format_date($row->timestamp),
      );
    }
    $watchdog_table = theme('table', array('header' => $watchdog_head, 'rows' => $watchdog_rows));
  }

  $queue_markup = '';

  if (variable_get('os2web_esdh_provider_queue_enabled', FALSE)) {
    // Status for external meeting importer.
    $queue_markup = '<h2>Meeting import: currently not running</h2>';

    if (!lock_acquire('os2web_esdh_provider_queue')) {
      $queue_markup = '<h2>Meeting import: running!</h2>';
    }
  }

  $os2web_esdh_provider_queue_form = drupal_get_form('os2web_esdh_provider_queue_form');
  $queue_markup .= drupal_render($os2web_esdh_provider_queue_form);
  $queue_markup .= '<h3>In queue</h3>';

  $queue_header = array(t('Publication id:'), t('uri'), t('Force'));
  $queue_rows = array();

  $queue_name = 'acadre_mm_import';
  $query = db_select('queue', 'q')
    ->fields('q', array('item_id', 'data', 'created'))
    ->condition('q.name', $queue_name)
    ->extend('PagerDefault')
    ->limit(25)
    ->execute();

  foreach ($query as $record) {
    $data = unserialize($record->data);
    if (isset($data['meeting'])) {
      $queue_rows[] = array(
        $data['meeting']['id'],
        $data['meeting']['uri'],
        $data['force'],
      );
    }
  }

  $queue_status = '<br /><hr /><br />' . $queue_markup . theme('table', array(
    'header' => $queue_header,
    'rows' => $queue_rows,
    'empty' => t('Queue is empty'),
  ));

  $queue_status .= '<br /><hr /><br />';

  $import_form = drupal_get_form('os2web_esdh_provider_import_form');

  return '<h2>' . t('ESDH Provider API plugin status') . '</h2>' .
  theme('table', array('header' => $head, 'rows' => $rows)) .
  '<h3>' . t('Last agenda import was successfully run at %time', array('%time' => format_date(variable_get('os2web_esdh_provider_last_import', 0)))) . '</h3>' .
  '<h3>' . t('Last import job was run at %time', array('%time' => format_date(variable_get('os2web_esdh_provider_last_mm_import', 0)))) . '</h3>' .
  drupal_render($import_form) . $queue_status . $watchdog_table;
}

/**
 * Queue form.
 */
function os2web_esdh_provider_queue_form($form_state) {
  $form = array();
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete queue'),
  );

  return $form;
}

/**
 * Submit handler for queue from.
 */
function os2web_esdh_provider_queue_form_submit(&$form, &$form_state) {
  // Empty queue.
  db_delete('queue')
    ->condition('name', 'acadre_mm_import')
    ->execute();

  drupal_set_message(t('Deleted queue'));
}

/**
 * Import form.
 */
function os2web_esdh_provider_import_form() {
  $form = array();
  $form['run_import'] = array(
    '#type' => 'submit',
    '#value' => t('Run import'),
  );
  $form['group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Extended options.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['group']['force'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force recreation of nodes'),
    '#default_value' => FALSE,
    '#description' => t('WARNING: This will destroy existing nodes and replace with freshly imported. All node-id\s will change'),
  );
  $form['group']['focus'] = array(
    '#type' => 'textfield',
    '#title' => t('Focused import id'),
    '#description' => t('Ie. 1577 to reimport XML_1577.xml.'),
  );
  $form['group']['debug_info'] = array(
    '#type' => 'checkbox',
    '#title' => t('Output debug info on import.'),
    '#description' => t('Generates an output from watchdog, and print timer reads in log.'),
    '#default_value' => variable_get('os2web_esdh_provider_debug_info', FALSE),
  );
  $form['group']['debug_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email address to send status on import.'),
    '#description' => t('The email to send status mail to.'),
    '#default_value' => variable_get('os2web_esdh_provider_debug_email'),
  );
  return $form;
}

/**
 * Implements hook_FORM_submit().
 */
function os2web_esdh_provider_import_form_submit($form, &$form_state) {
  variable_set('os2web_esdh_provider_debug_info', $form_state['values']['debug_info']);
  variable_set('os2web_esdh_provider_debug_email', $form_state['values']['debug_email']);
  drupal_goto('admin/config/os2web/esdh_provider/importconfirm', array(
    'query' => array(
      'id' => $form_state['values']['focus'],
      'force' => $form_state['values']['force'],
    ),
  ));
}

/**
 * Implements hook_form().
 */
function _os2web_esdh_provider_status_confirm($form, &$form_state) {
  $id = $_GET['id'];
  $force = (int) $_GET['force'];
  $form['_meeting_id'] = array(
    '#type' => 'hidden',
    '#value' => $id,
  );
  $form['_meeting_force'] = array(
    '#type' => 'hidden',
    '#value' => $force,
  );
  $msg = '';
  if ($id > 0) {
    $msg = t('<p>Importing meeting with id !focus <br/>', array('!focus' => $id));
  }
  else {
    $msg = t('<p>Importing all meetings.<br/>');
  }
  if ($force) {
    $msg .= t('All imported nodes will be re-imported, detroying existing data and importing new.<br/>');
  }
  else {
    $msg .= t('Only new nodes will be imported, existing data is preserved.</p>');
  }
  $msg .= t('<p>Are you sure you want to run this import?</p>');
  return confirm_form($form, t('Import meetings'), isset($_GET['destination']) ? $_GET['destination'] : "admin/config/os2web/esdh_provider", $msg, t('Run'), t('Cancel'));
}

/**
 * Implements hook_FORM_submit().
 */
function _os2web_esdh_provider_status_confirm_submit($form, &$form_state) {
  if (!os2web_esdh_provider_has_api('mm')) {
    drupal_set_message(t('ERROR: No valid MM importer plugin active!'), 'error');
    return;
  }

  $force = $form_state['values']['_meeting_force'] == 1;
  $focus = array_map('trim', explode(',', $form_state['values']['_meeting_id']));
  if (count($focus) === 1 && $focus[0] === '') {
    $focus = array();
  }

  $batch = array(
    'title' => 'Dagsorden import',
    'operations' => array(),
    'file' => drupal_get_path('module', 'os2web_esdh_provider') . '/os2web_esdh_provider.mmapi.inc',
    'init_message' => t('Scanner filer'),
    'finished' => 'os2web_esdh_provider_clear_cache',
  );

  $meetings = os2web_esdh_provider_invoke('mm', 'get_import_list');
  if (variable_get('os2web_esdh_provider_queue_enabled', FALSE)) {
    $queue = DrupalQueue::get('acadre_mm_import');

    foreach ($meetings as $meeting) {
      if (empty($focus) || in_array($meeting['id'], $focus)) {
        $queue->createItem(array('meeting' => $meeting, 'force' => $force));
      }
    }
    $queue->createItem(array('post_import_process' => TRUE));

    drupal_goto('admin/config/os2web/esdh_provider');
  }
  // Use normal batch processing if external queue is disabled.
  else {
    foreach ($meetings as $meeting) {
      $meeting['file_time'] = filemtime($meeting['uri']);
      if (empty($focus) || in_array($meeting['id'], $focus)) {
        $batch['operations'][] = array(
          '_os2web_esdh_provider_import_meeting',
          array($meeting, $force),
        );
      }
    }
    if (os2web_esdh_provider_supports('mm', 'post_import_process')) {
      $batch['operations'][] = array(
        'os2web_esdh_provider_invoke',
        array('mm', 'post_import_process'),
      );
    }

    batch_set($batch);
    // This isn't entirely neccesary, but needed to make the redirect work.
    // batch_process('admin/config/os2web/esdh_provider');.
  }
}

/**
 * Queue status table form.
 */
function _os2web_esdh_provider_queue_status_form($form, &$form_state) {
  $header = array(
    'meeting_name' => array(
      'data' => t('Title'),
      'type' => 'property',
      'field' => 'meeting_name',
    ),
    'meeting_node_date' => array(
      'data' => t('Meeting date'),
    ),
    'meeting_node_updated' => array(
      'data' => t('Node updated'),
    ),
    'meeting_xmlid' => array(
      'data' => t('XML ID'),
      'type' => 'property',
      'field' => 'meeting_xmlid',
      'sort' => 'desc'
    ),
    'meeting_xml_changed' => array(
      'data' => t('Meeting XML changed'),
      'type' => 'property',
      'field' => 'meeting_xml_changed',
    ),
    'committee' => array(
      'data' => t('Committee'),
      'type' => 'property',
      'field' => 'committeed_tid',
    ),
    'agenda_id' => array(
      'data' => t('Agenda SYS ID / XML ID'),
      'type' => 'property',
      'field' => 'agenda_sysid',
    ),
    'agenda_xml_changed' => array(
      'data' => t('Agenda XML changed'),
      'type' => 'property',
      'field' => 'agenda_xml_changed',
    ),
    'agenda_type' => array(
      'data' => t('Agenda Type'),
      'type' => 'property',
      'field' => 'agenda_type',
    ),
    'agenda_discovered' => array(
      'data' => t('Agenda discovered'),
      'type' => 'property',
      'field' => 'agenda_discovered',
    ),
    'agenda_processed' => array(
      'data' => t('Agenda processed'),
      'type' => 'property',
      'field' => 'agenda_processed',
    ),
    'import_status' => array(
      'data' => t('Import status'),
      'type' => 'property',
      'field' => 'import_status',
    ),
  );

  $query = db_select('os2web_esdh_provider_import', 'i')
    ->fields('i');
  $query->leftJoin('field_data_field_os2web_meetings_date', 'date', 'date.entity_id=i.nid');

  $query = $query->extend('PagerDefault')
    ->extend('TableSort')
    ->limit(50)
    ->orderByHeader($header);

  $search_params = array();
  if (!empty($_GET['committee_filter'])) {
    $search_params['committee_filter'] = $_GET['committee_filter'];
    $query->condition('committeed_tid', $search_params['committee_filter']);
  }
  if (!empty($_GET['start_date_filter'])) {
    $search_params['start_date_filter'] = implode(' ', $_GET['start_date_filter']);
    $start_date = date('Y-m-d H:i:s', strtotime($search_params['start_date_filter']));
    $query->condition('date.field_os2web_meetings_date_value', $start_date, '>=');
  }
  if (!empty($_GET['end_date_filter'])) {
    $search_params['end_date_filter'] = implode(' ', $_GET['end_date_filter']);
    $end_date = date('Y-m-d H:i:s', strtotime($search_params['end_date_filter']));
    $query->condition('date.field_os2web_meetings_date_value', $end_date, '<=');
  }
  if (!empty($_GET['xml_id_filter'])) {
    $search_params['xml_id_filter'] = $_GET['xml_id_filter'];
    $query->condition('i.meeting_xmlid', $search_params['xml_id_filter'] . '%', 'LIKE');
  }

  $result = $query->execute()->fetchAll();

  // Loading all nodes that are related, and committees.
  $nids = array();
  $tids = array();
  foreach ($result as $entry) {
    $nids[] = $entry->nid;
    $tids[] = $entry->committeed_tid;
  }
  $nodes = node_load_multiple($nids);
  $committees = taxonomy_term_load_multiple($tids);

  $options = array();
  foreach ($result as $entry) {
    $node = array_key_exists($entry->nid, $nodes) ? $nodes[$entry->nid] : NULL;
    $committee = array_key_exists($entry->committeed_tid, $committees) ? $committees[$entry->committeed_tid] : NULL;

    $import_status = $entry->import_status;
    $class = '';
    if (!$entry->agenda_processed && $entry->import_status != 'Ignored') {
      $import_status = 'Failed';
      $class = 'error';
    }
    $options[] = array(
      'meeting_name' => ($node) ? '<a href="' . url('node/' . $node->nid) . '">' . $node->title . '</a>' : $entry->meeting_name,
      'meeting_node_date' => ($node) ? $node->field_os2web_meetings_date['und'][0]['value'] : '',
      'meeting_node_updated' => ($node) ? format_date($node->changed, 'short') : '',
      'meeting_xmlid' => $entry->meeting_xmlid,
      'meeting_xml_changed' => format_date($entry->meeting_xml_changed, 'short'),
      'committee' => ($committee) ? $committee->name : '',
      'agenda_id' => $entry->agenda_sysid . ' / ' . $entry->agenda_xmlid,
      'agenda_xml_changed' => format_date($entry->agenda_xml_changed, 'short'),
      'agenda_type' => $entry->agenda_type,
      'agenda_discovered' => format_date($entry->agenda_discovered, 'short'),
      'agenda_processed' => format_date($entry->agenda_processed, 'short'),
      'import_status' => $import_status,
      '#attributes' => array('class' => array($class)),
    );
  }

  /* Filter START */
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic Search'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($search_params),
  );

  $vocabulary = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_committee');
  $terms = entity_load('taxonomy_term', FALSE, array('vid' => $vocabulary->vid));
  $terms_options = array();

  foreach ($terms as $term) {
    $terms_options[$term->tid] = $term->name;
  }

  $form['search']['committee_filter'] = array(
    '#type' => 'select',
    '#title' => t('Committee filter'),
    '#options' => $terms_options,
    '#empty_value' => 0,
    '#default_value' => !empty($search_params['committee_filter']) ? $search_params['committee_filter'] : 0,
  );

  $form['search']['start_date_filter'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start date filter'),
    '#default_value' => isset($search_params['start_date_filter']) ? date('Y-m-d H:i:s', strtotime($search_params['start_date_filter'])) : '',
  );

  $form['search']['end_date_filter'] = array(
    '#type' => 'date_popup',
    '#title' => t('End date filter'),
    '#default_value' => isset($search_params['end_date_filter']) ? date('Y-m-d H:i:s', strtotime($search_params['end_date_filter'])) : '',
  );

  $form['search']['xml_id_filter'] = array(
    '#type' => 'textfield',
    '#title' => t('XML ID filter'),
    '#default_value' => isset($search_params['xml_id_filter']) ? $search_params['xml_id_filter'] : '',
  );

  $form['search']['search_submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );
  $form['search']['search_reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );
  /* Filter END */

  $form['entities'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#attributes' => array('class' => array('entity-sort-table')),
    '#empty' => t('No content.'),
  );
  $form['pager'] = array('#theme' => 'pager');

  return $form;
}

/**
 * Queue status table form submit.
 */
function _os2web_esdh_provider_queue_status_form_submit($form, &$form_state) {
  $query = array();

  // If submitted via search button - populate the search queury.
  if ($form_state['triggering_element']['#id'] == 'edit-search-submit') {
    $values = $form_state['input'];

    $query = array();
    if (!empty($values['committee_filter'])) {
      $query['committee_filter'] = $values['committee_filter'];
    }
    if (isset($values['start_date_filter']) && !empty($values['start_date_filter']['date'])) {
      $query['start_date_filter'] = $values['start_date_filter'];
    }
    if (isset($values['end_date_filter']) && !empty($values['end_date_filter']['date'])) {
      $query['end_date_filter'] = $values['end_date_filter'];
    }
    if (isset($values['xml_id_filter'])) {
      $query['xml_id_filter'] = $values['xml_id_filter'];
    }
  }

  drupal_goto('admin/config/os2web/esdh_queue_status', array(
    'query' => $query
  ));
}
