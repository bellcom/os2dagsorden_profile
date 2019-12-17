function add_annotator(meeting_id, bullet_point_id, bilag_id, element_to_annotate, url, filter) {
  jQuery(document).ready(function() {

    var pageUri = function () {
    return {
        beforeAnnotationCreated: function (ann) {
            	ann.bilag_id= bilag_id;
              ann.bullet_point_id= bullet_point_id;
              ann.meeting_id= meeting_id;
        },
    };
};
    var app = new annotator.App();
    app.include(annotator.ui.main, {element: document.getElementById(element_to_annotate)})
     .include(annotator.storage.http, {
       prefix: url,
       urls: {
			  create:  'annotator/create',
			  read:    'annotator/read/:id',
			  update:  'annotator/update/:id',
			  destroy: 'annotator/delete/:id',
			  search:  'annotator/search'
			}
     })
    .include(annotatorImageSelect, {
        element: jQuery('#' + element_to_annotate +' img'),
    })
    .include(pageUri);
    app.start()
   .then(function () {
       app.annotations.load({
         bilag_id : bilag_id,
				 bullet_point_id : bullet_point_id,
				 meeting_id : meeting_id,
       });
   });

        if (jQuery(".bilag-list-btn").length) {
          jQuery(".bilag-list-btn").click(function(){
            jQuery(".bilag-list").toggle();
          });
        }
  });
  jQuery(window).load(function() {
    // Scroll bar on attachment page.
        if (jQuery(".annotator-outline-pager").length && Drupal.settings.os2dagsorden_annotator.attachment_add_scrollbar) {
          if (Drupal.settings.os2dagsorden_annotator.attachment_add_scrollbar == 'attachment_add_scrollbar_both') {
            jQuery("#content.has-outline-pager").css('overflow-y', 'auto');
            jQuery(".annotator-outline-pager").css('overflow-y', 'auto');
            jQuery(".annotator-outline-pager").css('overflow-x', 'hidden');
            var height = jQuery(window).height();
            jQuery('#content.has-outline-pager').innerHeight(height);
            jQuery(".annotator-outline-pager").innerHeight(height);
          }
          else if (Drupal.settings.os2dagsorden_annotator.attachment_add_scrollbar == 'attachment_add_scrollbar_attachment') {
          var pager_height = jQuery(".annotator-outline-pager").outerHeight();
          var window_height = jQuery(window).height();
          var height = (pager_height > window_height)? pager_height : window_height;
          jQuery('#content.has-outline-pager').innerHeight(height);
          jQuery("#content.has-outline-pager").css('overflow-y', 'auto');
        }
        else if (Drupal.settings.os2dagsorden_annotator.attachment_add_scrollbar == 'attachment_add_scrollbar_thumbnails') {
          var attachment_height = jQuery("#content.has-outline-pager").outerHeight();
          var window_height = jQuery(window).height();
          var height = (attachment_height > window_height)? attachment_height : window_height;
          jQuery('.annotator-outline-pager').innerHeight(height);
          jQuery(".annotator-outline-pager").css('overflow-y', 'auto');
        }
      }
  });
}

function annotator_hide_menu(){
	jQuery(document).ready(function(){
		jQuery(".region-sidebar-second-inner").hide();
		jQuery("#show_hide_menu_button").val("⇒");
		jQuery("#region-content").removeClass("grid-18");
		jQuery("#region-content").addClass("grid-24");
	});
}

function annotator_add_floatinscrollbar(){
	jQuery(document).ready(function(){
		jQuery("#pdf-main").floatingScrollbar();
	});
}

function highlight_wrapper(element, searchParam){
    jQuery(document).ready(function() {
        jQuery(element).highlight(searchParam);
    });
}
