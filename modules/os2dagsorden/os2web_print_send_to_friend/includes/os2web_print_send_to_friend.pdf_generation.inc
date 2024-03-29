<?php

/**
 * @file
 * Os2web_print_send_to_friend_pdf_generate.
 */

/**
 * Function that generates a PDF version of meeting agenda.
 *
 * Can generate with or without PDF attachments.
 *
 * Internally uses mPDF library to compose a PDF file.
 *
 * In the end of the function uses print_pdf_dispose_content to return the file
 * with correct headers.
 *
 * @param int $meeting_id
 *   Meeting ID.
 * @param bool $with_attachments
 *   TRUE if attachments must be added, otherwise - FALSE.
 */
function os2web_print_send_to_friend_pdf_generate($meeting_id, $with_attachments = TRUE) {
  require_once DRUPAL_ROOT . '/profiles/os2dagsorden/libraries/mpdf/vendor/autoload.php';
  // Set up and prepare mPDF tool.
  module_load_include('inc', 'print', 'includes/print');

  $pdf_tool = explode('|', variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT));
  if (empty($paper_size)) {
    $paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
  }
  if (empty($page_orientation)) {
    $page_orientation = variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT);
  }

  if (variable_get('print_pdf_autoconfig', PRINT_PDF_AUTOCONFIG_DEFAULT)) {
    if (!defined('_MPDF_TTFONTDATAPATH')) {
      define('_MPDF_TTFONTDATAPATH', drupal_realpath('public://print_pdf/print_pdf_mpdf/ttfontdata/'));
    }
    if (!defined('_MPDF_TEMP_PATH')) {
      define('_MPDF_TEMP_PATH', drupal_realpath('public://print_pdf/print_pdf_mpdf/tmp/'));
    }
  }

  require_once DRUPAL_ROOT . '/' . $pdf_tool[1];

  $format = ($page_orientation === "landscape") ? $paper_size . "-L" : $paper_size;

  // Set document information.
  $mpdf = new mPDF('UTF-8', $format);
  $mpdf->SetCreator(variable_get('site_name', 'Drupal'));

  // Need this in order to import PDF's.
  $mpdf->SetImportUse();

  // Add the css
  // external css.
  $stylesheet = file_get_contents(DRUPAL_ROOT . '/profiles/os2dagsorden/modules/os2dagsorden/os2web_print_send_to_friend/css/os2web_print_send_to_friend_pdf.css');
  $mpdf->WriteHTML($stylesheet, 1);

  // Start the real output.
  $meeting = node_load($meeting_id);
  $mpdf = os2web_print_send_to_friend_generate_pdf_meeting_general_info($meeting, $mpdf);

  if (isset($meeting->field_os2web_meetings_bullets['und'])) {
    for ($i = 0; $i < count($meeting->field_os2web_meetings_bullets['und']); $i++) {
      $bp_id = $meeting->field_os2web_meetings_bullets['und'][$i]['target_id'];

      $is_last_bp = ($i === count($meeting->field_os2web_meetings_bullets['und']) - 1);

      if (os2dagsorden_access_helper_check_user_has_access_to_bullet_point($meeting_id, $bp_id)) {
        $mpdf = os2web_print_send_to_friend_generate_pdf_bullet_point($bp_id, $mpdf, $is_last_bp, $with_attachments);
      }
    }
  }

  // Try to recover from any warning/error.
  ob_clean();

  print_pdf_dispose_content($mpdf->Output('', 'S'), $meeting->title . '.pdf');
}

/**
 * Fills the provided mPDF object with meeting general info (as HTML).
 *
 * @param object $meeting
 *   Meeting object.
 * @param object $mpdf
 *   mPDF object.
 *
 * @return mixed
 *   Updated mPDF object.
 */
function os2web_print_send_to_friend_generate_pdf_meeting_general_info($meeting, $mpdf) {
  $start_date = $meeting->field_os2web_meetings_date['und'][0]['value'];
  $start_date_obj = date_create($start_date, new DateTimeZone('UTC'));
  date_timezone_set($start_date_obj, new DateTimeZone(date_default_timezone()));
  $start_date = date_format($start_date_obj, 'Y-m-d H:i:s');

  $end_date = $meeting->field_os2web_meetings_date['und'][0]['value2'];
  $end_date_obj = date_create($end_date, new DateTimeZone('UTC'));
  date_timezone_set($end_date_obj, new DateTimeZone(date_default_timezone()));
  $end_date = date_format($end_date_obj, 'Y-m-d H:i:s');

  $committee = taxonomy_term_load($meeting->field_os2web_meetings_committee['und'][0]['tid']);
  $location = taxonomy_term_load($meeting->field_os2web_meetings_location['und'][0]['tid']);

  $mpdf->WriteHTML(
    "<table>
      <thead>
        <tr>
          <th>" . t('Meeting') . "</th>
          <th>" . t('Startdato') . "</th>
          <th>" . t('Slutdato') . "</th>
          <th>" . t('Udvalg') . "</th>
          <th>" . t('Location') . "</th>
        </tr>
      </thead>
      <tbody>
        <tr>
          <td>$meeting->title</td>
          <td>$start_date</td>
          <td>$end_date</td>
          <td>$committee->name</td>
          <td>$location->name</td>
        </tr>
      </tbody>
    </table>"
  );

  return $mpdf;
}

/**
 * Fills the provided mPDF object with bullet point data.
 *
 * If with_attachments is set to TRUE, all PDF attachments are included as well.
 *
 * @param int $bullet_point_id
 *   Bullet point id.
 * @param object $mpdf
 *   mPDF object.
 * @param bool $is_last_bp
 *   TRUE or FALSE.
 * @param bool $with_attachments
 *   TRUE or FALSE.
 *
 * @return mixed
 *   Updated mPDF object.
 */
function os2web_print_send_to_friend_generate_pdf_bullet_point($bullet_point_id, $mpdf, $is_last_bp, $with_attachments = TRUE) {
  $bullet_point = node_load($bullet_point_id);
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->fieldCondition('field_os2web_meetings_bullets', 'target_id', $bullet_point_id, '=');
  $result = $query->execute();
  if (isset($result['node'])) {
    $nids = array_keys($result['node']);
    $meeting_id = reset($nids);
  }

  if (variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE)) {
    $publish_level = intval($bullet_point->field_os2web_meetings_bul_plevel['und'][0]['value']);
    $title = os2dagsorden_access_helper_get_bullet_point_title($meeting_id, $bullet_point_id, $bullet_point->title, $publish_level);
  }
  else {
    $title = $bullet_point->title;
  }

  if (strcmp($bullet_point->type, 'os2web_meetings_bullet') === 0) {
    $mpdf->WriteHtml('<h2>' . $title . '</h2>');

    // Eliminating duplicates in bullet points attachments.
    $attachments = array();
    if ($bullet_point->field_os2web_meetings_attach['und']) {
      foreach ($bullet_point->field_os2web_meetings_attach['und'] as $attachment_id) {
        if (os2dagsorden_access_helper_check_user_can_see_bullet_point_attachment($meeting_id, $bullet_point_id, $attachment_id['target_id'])) {
          $hash = $attachment_id['target_id'];
          $attachments[$hash] = $attachment_id;
        }
      }
    }

    // Help flag to add a new page after PDF attachmed has been imported.
    $newPortaitPageRequired = FALSE;

    // Looping though each attachment.
    foreach ($attachments as $attachment_id) {
      if ($newPortaitPageRequired) {
        $mpdf->addPage('P');
        $newPortaitPageRequired = FALSE;
      }

      $attachment = node_load($attachment_id['target_id']);
      if (variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE)) {
        $publish_level = intval($attachment->field_os2web_meetings_bpa_plevel['und'][0]['value']);
        $title = os2dagsorden_access_helper_get_bilag_title($meeting_id, $bullet_point->nid, $attachment->title, $publish_level);
      }
      else {
        $title = $attachment->title;
      }

      $mpdf->WriteHtml('<h3>' . $title . '</h3>');

      if (!empty($attachment->field_os2web_meetings_bpa_body['und'][0]['value'])) {
        $mpdf->WriteHtml(preg_match("|^<p>(.*)</p>|", $attachment->field_os2web_meetings_bpa_body['und'][0]['value']) ? $attachment->field_os2web_meetings_bpa_body['und'][0]['value'] : "<p>" . $attachment->field_os2web_meetings_bpa_body['und'][0]['value'] . "</p>");
      }
      elseif ($with_attachments && $attachment->field_os2web_meetings_bpa_file['und'][0]['fid']) {
        $uri = $attachment->field_os2web_meetings_bpa_file['und'][0]['uri'];
        // Quick check without file loading if the file is PDF.
        if (strcasecmp(pathinfo($uri, PATHINFO_EXTENSION), 'pdf') === 0) {
          if (file_exists($uri)) {
            try {
              // Import file and include it into the output.
              $pagecount = $mpdf->SetSourceFile(drupal_realpath($uri));
              for ($i = 1; $i <= $pagecount; $i++) {
                $tplidx = $mpdf->importPage($i);
                $specs = $mpdf->getTemplateSize($tplidx);
                $mpdf->addPage($specs['h'] > $specs['w'] ? 'P' : 'L');
                $mpdf->useTemplate($tplidx);
              }
              $newPortaitPageRequired = TRUE;
            }
            catch (Exception $e) {
              watchdog('Print send to friend', 'This file !uri cannot be included. Maybe it is still not converted to PDF 1.4? Failed with this message: !message. ',
                array('!uri' => $uri, '!message' => $e->getMessage()), WATCHDOG_WARNING);
            }
          }
        }
      }
    }
  }

  // Making sure a new page is added.
  if (!$is_last_bp) {
    $mpdf->addPage('P');
  }

  return $mpdf;
}

/**
 * Function that generates a PDF version of bullet point.
 *
 * Can generate with or without PDF attachments.
 *
 * Internally uses mPDF library to compose a PDF file.
 *
 * In the end of the function uses print_pdf_dispose_content to return the file
 * with correct headers.
 *
 * @param int $bp_id
 *   Meeting ID.
 * @param bool $with_attachments
 *   TRUE if attachments must be added, otherwise - FALSE.
 */
function os2web_print_send_to_friend_bp_pdf_generate($bp_id, $with_attachments = TRUE) {
  require_once DRUPAL_ROOT . '/profiles/os2dagsorden/libraries/mpdf/vendor/autoload.php';
  $bullet_point = node_load($bp_id);
  // Set up and prepare mPDF tool.
  module_load_include('inc', 'print', 'includes/print');

  $pdf_tool = explode('|', variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT));
  if (empty($paper_size)) {
    $paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
  }
  if (empty($page_orientation)) {
    $page_orientation = variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT);
  }

  if (variable_get('print_pdf_autoconfig', PRINT_PDF_AUTOCONFIG_DEFAULT)) {
    if (!defined('_MPDF_TTFONTDATAPATH')) {
      define('_MPDF_TTFONTDATAPATH', drupal_realpath('public://print_pdf/print_pdf_mpdf/ttfontdata/'));
    }
    if (!defined('_MPDF_TEMP_PATH')) {
      define('_MPDF_TEMP_PATH', drupal_realpath('public://print_pdf/print_pdf_mpdf/tmp/'));
    }
  }

  require_once DRUPAL_ROOT . '/' . $pdf_tool[1];

  $format = ($page_orientation === "landscape") ? $paper_size . "-L" : $paper_size;

  // Set document information.
  $mpdf = new mPDF('UTF-8', $format);
  $mpdf->SetCreator(variable_get('site_name', 'Drupal'));

  // Need this in order to import PDF's.
  $mpdf->SetImportUse();

  // Add the css
  // external css.
  $stylesheet = file_get_contents(DRUPAL_ROOT . '/profiles/os2dagsorden/modules/os2dagsorden/os2web_print_send_to_friend/css/os2web_print_send_to_friend_pdf.css');
  $mpdf->WriteHTML($stylesheet, 1);
  $mpdf = os2web_print_send_to_friend_generate_pdf_bullet_point($bp_id, $mpdf, true, $with_attachments);

  // Try to recover from any warning/error.
  ob_clean();

  print_pdf_dispose_content($mpdf->Output('', 'S'), $bullet_point->title . '.pdf');
}
