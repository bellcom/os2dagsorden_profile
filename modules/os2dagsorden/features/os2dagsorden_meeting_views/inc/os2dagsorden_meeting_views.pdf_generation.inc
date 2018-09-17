<?php

/**
 * @file
 * Os2dagsorden_meeting_view.pdf_genration.
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
function os2dagsorden_meeting_views_meetings_overview_pdf_generate() {
  require_once DRUPAL_ROOT . '/profiles/os2dagsorden/libraries/mpdf/vendor/autoload.php';

  // Set up and prepare mPDF tool.
  module_load_include('inc', 'print', 'includes/print');

  $pdf_tool = explode('|', variable_get('print_pdf_pdf_tool', PRINT_PDF_PDF_TOOL_DEFAULT));
  if (empty($paper_size)) {
    $paper_size = variable_get('print_pdf_paper_size', PRINT_PDF_PAPER_SIZE_DEFAULT);
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

  $format = $paper_size . "-P";

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
  $view = views_get_view('meetings_search');
  $view->set_display('page_meetings_overview');
  $params = drupal_get_query_parameters();
  $view->set_exposed_input($params);
  $view->execute();
  $results = $view->result;

  foreach($results as $result) {
    $meeting = node_load($result->nid);
    $rendered_meeting = node_view($meeting, 'os2dagsorden_meeting_overview');
    $mpdf->WriteHTML(render($rendered_meeting));
  }

  // Try to recover from any warning/error.
  ob_clean();

  print_pdf_dispose_content($mpdf->Output('', 'S'), $meeting->title . '.pdf');
}