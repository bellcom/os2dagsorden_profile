<?php

/**
 * @file
 * Os2dagsorden_annotator.
 *
 * PHP version 5
 *
 * @category OS2Dagsorden
 * @package OS2Dagsorden_Annotator
 *  The annotator module help functions, which might be used by another modules
 *  or views.
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/licenses/gpl-2.0.html GNU General Public License,
 *  version 2
 * @link http://bellcom.dk
 */

/**
 * Returns the notes using the attachment id.
 *
 * @param int $bilag_id
 *   Attachment id.
 *
 * @return mixed
 *   Array of notes.
 */
function os2dagsorden_annotator_get_notes_by_attachment_id($bilag_id) {
  global $user;

  $result = db_select('os2dagsorden_annotator_notes', 'notes')
    ->fields('notes')
    ->condition('uid', $user->uid, '=')
    ->condition('bilag_id', $bilag_id, '=')
    ->execute();

  // Looping through notes, adding them to array.
  $notes = array();
  while ($record = $result->fetchAssoc()) {
    $note_arr = json_decode($record['note_info']);

    // Adding missing fields.
    $note_arr->id = $record['id'];
    $note_arr->bilag_id = $record['bilag_id'];
    $note_arr->bullet_point_id = $record['bullet_point_id'];
    $note_arr->meeting_id = $record['meeting_id'];

    $note_arr->user = $user->name;
    $note_arr->permissions = array();

    $notes[] = $note_arr;
  }

  return $notes;
}

/**
 * Returns the notes using the bullet point id.
 *
 * @param int $bullet_point_id
 *   Bullet point id.
 *
 * @return mixed
 *   Array of notes.
 */
function os2dagsorden_annotator_get_notes_by_bullet_point_id($bullet_point_id) {
  global $user;

  $result = db_select('os2dagsorden_annotator_notes', 'notes')
    ->fields('notes')
    ->condition('uid', $user->uid, '=')
    ->condition('bullet_point_id', $bullet_point_id, '=')
    ->execute();

  // Looping through notes, adding them to array.
  $notes = array();
  while ($record = $result->fetchAssoc()) {
    $note_arr = json_decode($record['note_info']);

    // Adding missing fields.
    $note_arr->id = $record['id'];
    $note_arr->bilag_id = $record['bilag_id'];
    $note_arr->bullet_point_id = $record['bullet_point_id'];
    $note_arr->meeting_id = $record['meeting_id'];

    $note_arr->user = $user->name;
    $note_arr->permissions = array();

    $notes[] = $note_arr;
  }

  return $notes;
}

/**
 * Returns the notes using the meeting id.
 *
 * @param int $meeting_id
 *   Meeting id.
 *
 * @return mixed
 *   Array of notes.
 */
function os2dagsorden_annotator_get_notes_by_meeting_id($meeting_id) {
  global $user;

  $notes = array();
  $possible_meetings = os2dagsorden_access_helper_get_meeting_and_tillaegs_meetings($meeting_id);

  foreach ($possible_meetings as $nid) {
    $result = db_select('os2dagsorden_annotator_notes', 'notes')
      ->fields('notes')
      ->condition('uid', $user->uid, '=')
      ->condition('meeting_id', $nid, '=')
      ->execute();

    // Looping through notes, adding them to array.
    while ($record = $result->fetchAssoc()) {
      $note_arr = json_decode($record['note_info']);

      // Adding missing fields.
      $note_arr->id = $record['id'];
      $note_arr->bilag_id = $record['bilag_id'];
      $note_arr->bullet_point_id = $record['bullet_point_id'];
      $note_arr->meeting_id = $meeting_id;

      $note_arr->user = $user->name;
      $note_arr->permissions = array();

      $notes[] = $note_arr;
    }
  }

  return $notes;
}

/**
 * Function that updates the reference of meeting nid.
 *
 * @param int $old_meeting_nid
 *   Old meeting nid.
 * @param int $new_meeting_nid
 *   New meeting nid.
 */
function os2dagsorden_annotator_update_notes_meeting($old_meeting_nid, $new_meeting_nid) {
  db_update('os2dagsorden_annotator_notes')
    ->fields(array(
      'meeting_id' => $new_meeting_nid,
    ))
    ->condition('meeting_id', $old_meeting_nid, '=')
    ->execute();
}

/**
 * Function that updates the reference of bilag nid.
 *
 * @param int $old_attachment_nid
 *   Old nid of the attachment.
 * @param int $new_attachment_nid
 *   New nid of the attachment.
 */
function os2dagsorden_annotator_update_notes_attachment($old_attachment_nid, $new_attachment_nid) {
  $old_attachment = node_load($old_attachment_nid);
  $new_attachment = node_load($new_attachment_nid);

  // Bullet point attachment body changed
  // needs notes update.
  if (strcasecmp($old_attachment->field_os2web_meetings_bpa_body[['und']][0]['value'],
        $new_attachment->field_os2web_meetings_bpa_body['und'][0]['value']) != 0) {

    $result = db_select('os2dagsorden_annotator_notes', 'notes')
      ->fields('notes')
      ->condition('bilag_id', $old_attachment_nid, '=')
      ->execute();
    while ($record = $result->fetchAssoc()) {
      $bpa_body = $new_attachment->field_os2web_meetings_bpa_body['und'][0]['value'];
      $bpa_body = preg_replace('#<(meta)(?:[^>]+)?>.*?#s', '', $bpa_body);
      $bpa_body = preg_replace('#<(style)(?:[^>]+)?>.*?</\1>#s', '', $bpa_body);

      // First word.
      $text = strip_tags($bpa_body);
      $text = preg_replace("/&nbsp;/", ' ', $text);
      $text = preg_replace('/[\n\r]/', ' ', $text);
      $text = trim($text);
      $first_word = mb_strstr($text, ' ', TRUE);

      // First word path.
      $doc = new DOMDocument();
      $doc->loadHTML($bpa_body);
      $path = _os2dagsorden_annotator_find_path($doc, $first_word);
      $path = str_replace('/html/body/', '/div/', $path);
      // ranges.
      $text = strip_tags($bpa_body);
      $text = trim($text);
      $start = strpos($text, $first_word);
      $end = strlen($first_word) + $start;

      $ranges = json_decode($record["note_info"]);
      $ranges->quote = $first_word;
      $ranges->ranges[0]->startOffset = $start;
      $ranges->ranges[0]->start = $path;
      $ranges->ranges[0]->end = $path;
      $ranges->ranges[0]->endOffset = $end;

      db_update('os2dagsorden_annotator_notes')
        ->fields(array(
          'note_info' => json_encode($ranges),
          'bilag_id' => $new_attachment_nid,
        ))
        ->condition('id', $record['id'], '=')
        ->execute();
    }
  }
  else {
    db_update('os2dagsorden_annotator_notes')
      ->fields(array(
        'bilag_id' => $new_attachment_nid,
      ))
      ->condition('bilag_id', $old_attachment_nid, '=')
      ->execute();
  }
}

/**
 * Returns the right DOM path for the certain text, is used recursively.
 *
 * @param object $domNode
 *   DOMNode.
 * @param string $first_word
 *   The first word.
 * @param string $current_path
 *   The current path.
 *
 * @return string
 *   Path of the text.
 */
function _os2dagsorden_annotator_find_path($domNode, $first_word, $current_path = '') {
  foreach ($domNode->childNodes as $node) {
    if ($node->nodeName === 'style') {
      continue;
    }
    if ($node->nodeType === XML_ELEMENT_NODE) {
      if ($node->hasChildNodes()) {
        return _os2dagsorden_annotator_find_path($node, $first_word, $current_path . '/' . $node->nodeName);
      }
    }
    elseif ($node->nodeType === XML_TEXT_NODE) {

      $text = strip_tags($node->nodeValue);
      $text = utf8_decode($text);
      $text = preg_replace("/&nbsp;/", '', $text);
      $text = trim($text);

      if ($text != '') {
        if (substr($text, 0, strlen($first_word)) === $first_word) {
          return $current_path;
        }
      }
    }
  }
}
