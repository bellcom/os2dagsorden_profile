jQuery(function($) {
  'use strict';

  // Sidr
  $('.responsive-button--sidr-toggle').sidr({
    name: 'sidr-main',
    side: 'right',
    renaming: false,
    source: '.sidr-source-provider'
  });

  // Responsive additions.
  var $page = $('#page');
  var $responsiveHeader = $('<div />').addClass('responsive-header');
  var $searchButton = $('<a />').attr('href', '/meetings-search').addClass('responsive-button responsive-button--search');
  var $toggleSidrButton = $('<button />').addClass('responsive-button responsive-button--sidr-toggle');

  //$page.prepend($responsiveHeader);

  // Force open the bulletpoint attachments by clicking
  // the "toggle all" on mobile. The button is visually
  // hidden on mobile by CSS, but is visible on
  // desktop.
  if (window.outerWidth < 767) {

    if (document.body.classList.contains('page-meeting')) {
      var toggleAllButton = document.querySelector('.hide_show_all_attachments_text.top');

      if (! toggleAllButton.classList.contains('opened')) {
        toggleAllButton.click();
      }
    }
  }
});

function is_touch_device() {
  var prefixes = ' -webkit- -moz- -o- -ms- '.split(' ');
  var mq = function(query) {
    return window.matchMedia(query).matches;
  }

  if (('ontouchstart' in window) || window.DocumentTouch && document instanceof DocumentTouch) {
    return true;
  }

  // include the 'heartz' as a way to have a non matching MQ to help terminate the join
  // https://git.io/vznFH
  var query = ['(', prefixes.join('touch-enabled),('), 'heartz', ')'].join('');

  return mq(query);
}

if (is_touch_device()) {
  var root = document.getElementsByTagName( 'html' )[0]; // '0' to assign the first (and only `HTML` tag)

  root.classList.add('has-touch');
}
