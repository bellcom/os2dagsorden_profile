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
      const toggleAllButton = document.querySelector('.hide_show_all_attachments_text.top');

      if (! toggleAllButton.classList.contains('opened')) {
        toggleAllButton.click();
      }
    }
  }
});
