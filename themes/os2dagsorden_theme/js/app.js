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
});
