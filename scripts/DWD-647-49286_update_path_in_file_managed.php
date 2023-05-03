<?php
$result = db_query("SELECT f.* FROM {file_managed} f");
$count = 0;
  foreach ($result as $file) {
    if($file->uri) {
      $url = str_replace('//Attachments', '/Attachments', $file->uri, $replacments);
      if ($replacments) {
        // Updating database entry.
        db_update('file_managed')
          ->fields(array(
            'uri' => $url,
          ))
          ->condition('fid', $file->fid)
          ->execute();
        $count++;
      }

    }
  }
print "done $count replacments" . PHP_EOL;
