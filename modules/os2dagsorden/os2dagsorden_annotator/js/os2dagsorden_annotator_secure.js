function add_annotator(meeting_id, bullet_point_id, bilag_id, element_to_annotate, url, filter) {
  jQuery(document).ready(function() {
    var hash = window.location.hash.substring(1);
    var pageUri = function () {
    return {
        beforeAnnotationCreated: function (ann) {
            	ann.bilag_id= bilag_id;
              ann.bullet_point_id= bullet_point_id;
              ann.meeting_id= meeting_id;
        },
        annotationsLoaded: function (ann) {
          if (hash && !(jQuery(".annotator-outline-pager").length && Drupal.settings.os2dagsorden_annotator.attachment_add_scrollbar)){
            jQuery("body, html").animate({
              scrollTop:  jQuery('span[data-annotation-id='+ hash +']').offset().top
            }, 100 );
          }
        }
      }
    }
    var app = new annotator.App();
    app.include(annotator.storage.http, {
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
    .include(annotator.touch.main, {element: document.getElementById(element_to_annotate)})
    .include(pageUri);
    app.start()
   .then(function () {
       app.annotations.load({
         bilag_id : bilag_id,
				 bullet_point_id : bullet_point_id,
				 meeting_id : meeting_id,
       });
   });

   if (!jQuery("body .annotator-touch-controls.dummy-controls").length ) {
            jQuery('body').append(
                '<div class="annotator-touch-widget annotator-touch-controls dummy-controls">' +
                    '<div class="annotator-touch-widget-inner">' +
                        '<a class="annotator-button annotator-add annotator-focus">' + Drupal.settings.os2dagsorden_annotator.annotator_button_text + '</a>' +
                    '</div>' +
                '</div>'
            );
            jQuery('body .annotator-touch-controls.dummy-controls').click(function(e) {
                e.preventDefault();
                if (jQuery("#ToolTipDiv2").css('display') != 'block') {
                  jQuery("#ToolTipDiv2").html("Marker tekst og klik herefter på Lav note").fadeIn(400);
                  setTimeout(function(){
                      jQuery("#ToolTipDiv2").fadeOut("slow");
                  },5000)
                }
                else {
                  jQuery("#ToolTipDiv2").css({'display': 'none'});
                }
            });
        }

        // Changing button text.
        jQuery(".annotator-button.annotator-add.annotator-focus").text(Drupal.settings.os2dagsorden_annotator.annotator_button_text);
        if (Drupal.settings.os2dagsorden_annotator.annotator_hide_text) {
            jQuery(".annotator-button.annotator-add.annotator-focus").html('&nbsp;');
            jQuery(".annotator-button.annotator-add.annotator-focus").css("cssText", "padding-right: 0 !important;");
        }
        if (Drupal.settings.os2dagsorden_annotator.annotator_use_title) {
            jQuery(".annotator-touch-widget-inner").attr("title", Drupal.settings.os2dagsorden_annotator.annotator_button_text);
        }

        if (jQuery(".bilag-list-btn").length) {
          jQuery(".bilag-list-btn").click(function(){
            jQuery(".bilag-list").toggle();
          });
        }
  });
  jQuery(window).load(function() {
    var hash = window.location.hash.substring(1);
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
        if (hash) {
            var offset = jQuery('span[data-annotation-id='+ hash +']').offset().top - jQuery('#content.has-outline-pager').offset().top - 40;
            jQuery("#content").animate({
              scrollTop:  offset
            }, 100 );
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
