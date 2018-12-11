<?php

/**
 * @file
 * Os2dagsorden_access_helper.print.inc.
 *
 * Various print related functions.
 *
 * @category OS2Dagsorden
 * @package OS2Dagsorden_Access_Helper
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/licenses/gpl-2.0.html GNU General Public License, version 2
 * @link http://bellcom.dk
 */

/**
 * Returns the nice print version of the speaker paper.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param int $speaker_paper_id
 *   ID of the speaker paper.
 *
 * @return string
 *   for nice printing of speaker paper.
 */
function os2dagsorden_access_helper_print_speaker_paper($bullet_point_id, $speaker_paper_id) {
  $html = os2dagsorden_access_helper_print_speaker_paper_generate_html($bullet_point_id, $speaker_paper_id);
  if (!empty($html)) {
    $speaker_paper = node_load($speaker_paper_id);
    drupal_set_title(variable_get('os2dagsorden_title_meeting_comments', 'Talepapir') . ' - ' . $speaker_paper->title);
    return ('<div class="node">' . $html . '</div>');
  }
}

/**
 * Generates HTML of meeting speaker papers.
 *
 * @param int $bullet_point_id
 *   ID of the bullet point.
 * @param int $speaker_paper_id
 *   ID of the speaker paper.
 *
 * @return string
 *   The html of the created page.
 */
function os2dagsorden_access_helper_print_speaker_paper_generate_html($bullet_point_id, $speaker_paper_id) {
  $html = '';
  if (os2dagsorden_access_helper_bullet_point_has_speaker_paper($bullet_point_id, variable_get('os2dagsorden_shared_speaker_paper', TRUE))) {
    $bullet_point = node_load($bullet_point_id);
    $speaker_paper = node_load($speaker_paper_id);

    $html = '<h3>' . $bullet_point->title . ' - ' . variable_get('os2dagsorden_title_meeting_comments', 'Talepapir') . ' - ' . $speaker_paper->title . '</h3>';

    $html .= '<div class="speaker-paper-content">' . $speaker_paper->body['und'][0]['value'] . '</div>';
  }
  return $html;
}
/**
 * Composes the view of all speaker papers for this meeting.
 *
 * @param int $meeting_id
 *   ID of the meeting.
 *
 * @return string
 *   The html of the created page.
 */
function os2dagsorden_access_helper_meeting_all_meeting_speaker_papers_view($meeting_id) {
  global $base_url;

  // Overriding title for bilag.
  //drupal_set_title(variable_get('os2dagsorden_title_meeting_comments', 'Talepapir'));

  // Breadcrumb.
  $breadcrumb[] = l(t('Forsiden'), $base_url);
  $breadcrumb[] .= l(t('Mødedetaljer'), 'meeting/' . $meeting_id);
  $breadcrumb[] .= '<span class="breadcrumb-active">' . variable_get('os2dagsorden_title_meeting_comments', 'Talepapir') .'</span>';
  drupal_set_breadcrumb($breadcrumb);

  $meeting = node_load($meeting_id);
  drupal_set_title(variable_get('os2dagsorden_title_meeting_comments', 'Talepapir') . ' - ' . $meeting->title);

  // Help icon.
  $html = '<div class="node">';
  $html .= '<div id="content" class="view-content">';

  // Print and send to friend.
  $print_link = l(variable_get('os2dagsorden_title_meeting_print_all_speakerpapers'), "print/meeting/$meeting_id/speaker-papers/print", array('attributes' => array('target' => '_blank', 'class' => array('form-submit'))));

  $html .= '<div class="controls-right no-print">' . $print_link . '</div>';
  $html .= '<div class="clearfix"></div>';

  $html .= os2dagsorden_access_helper_meeting_all_meeting_speaker_papers_generate_html($meeting);

  $html .= '</div></div>';
  return $html;
}
/**
 * Generates HTML of meeting speaker papers.
 *
 * @param mixed $meeting
 *   Meeting object.
 *
 * @return string
 *   The html of the created page.
 */
function os2dagsorden_access_helper_meeting_all_meeting_speaker_papers_generate_html($meeting) {
  global $user;
  $html = "";
  $bullet_point_unique = array();
  if (isset($meeting->field_os2web_meetings_bullets['und'])) {
    foreach ($meeting->field_os2web_meetings_bullets['und'] as $bullet_point) {
      $bullet_point_unique[] = $bullet_point['target_id'];
    }
    $bullet_point_unique = array_unique($bullet_point_unique);
    foreach ($bullet_point_unique as $bullet_point) {
      // User own speaker paper.
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', array('os2web_meetings_spaper'))
        ->propertyCondition('uid', $user->uid)
        ->fieldCondition('field_os2web_meetings_sp_bullet', 'target_id', $bullet_point, '=');

      $result = $query->execute();
      if (isset($result['node'])) {
        $nids = array_keys($result['node']);
        foreach ($nids as $nid) {
          $html .= os2dagsorden_access_helper_print_speaker_paper_generate_html($bullet_point, $nid);
        }
      }
      // Shared speaker papers.
      if (variable_get('os2dagsorden_shared_speaker_paper', TRUE)) {
        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'node')
          ->propertyCondition('type', array('os2web_meetings_spaper'))
          ->fieldCondition('field_os2web_meetings_sp_shared', 'target_id', $user->uid, '=')
          ->fieldCondition('field_os2web_meetings_sp_bullet', 'target_id', $bullet_point, '=');

        $result = $query->execute();
        if (isset($result['node'])) {
          $nids = array_keys($result['node']);
          foreach ($nids as $nid) {
            $html .= os2dagsorden_access_helper_print_speaker_paper_generate_html($bullet_point, $nid);
          }
        }
      }
    }
  }
  return $html;
}