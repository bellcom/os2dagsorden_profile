<?php

/**
 * @file
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Whether to show the calendar block or not, depending on the current path.
 */
function os2dagsorden_sidepane_menu_show_calendar_block() {
  $q = explode('/', $_GET['q']);
  if ($q[0] == 'calendar-field-date') {
    return FALSE;
  }
  return TRUE;
}
