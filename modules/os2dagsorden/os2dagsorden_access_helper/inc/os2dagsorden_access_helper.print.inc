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
  if (os2dagsorden_access_helper_bullet_point_has_speaker_paper($bullet_point_id, variable_get('os2dagsorden_shared_speaker_paper', TRUE))) {
    $bullet_point = node_load($bullet_point_id);
    $speaker_paper = node_load($speaker_paper_id);
    drupal_set_title(t('Talepapir') . ' - ' . $speaker_paper->title);
    $html = '<h3>' . $bullet_point->title . ' - Talepapir - ' . $speaker_paper->title . '</h3>';

    $html .= $speaker_paper->body['und'][0]['value'];
  }

  return ('<div class="node">' . $html . '</div>');
}

/**
 * Composes the print page for meeting's speaker papers for a current user.
 *
 * @param int $meeting_id
 *   Id of a meeting.
 *
 * @return mixed
 *   List of the speaker papers attached to that meeting.
 */
function os2dagsorden_access_helper_print_meeting_speaker_papers($meeting_id) {
  global $user;
  $speaker_papers_print = "";
  $meeting = node_load($meeting_id);
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
          $speaker_papers_print .= os2dagsorden_access_helper_print_speaker_paper($bullet_point, $nid);
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
            $speaker_papers_print .= os2dagsorden_access_helper_print_speaker_paper($bullet_point, $nid);
          }
        }
      }
    }
  }
  drupal_set_title(t('Talepapirer') . ' - ' . $meeting->title);
  return $speaker_papers_print;
}
