/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

(function ($) {
  Drupal.behaviors.os2dagsordenSettingsBehaviours = {
    attach: function (context, settings) {
      $("input[name='os2dagsorden_collapse_menu']").change(function () {
        localStorage.removeItem('hide_side_menu');
      });
       $("input[name='os2dagsorden_collapse_menu_touch']").change(function () {
        localStorage.removeItem('hide_side_menu');
      });

    }
  }
})(jQuery);