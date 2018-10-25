function add_annotator(meeting_id, bullet_point_id, bilag_id, element_to_annotate, url, filter) {
  jQuery(document).ready(function() {
		jQuery(element_to_annotate).annotator().annotator('addPlugin', 'Touch', {
			force: 1,
			useHighlighter: location.search.indexOf('highlighter') > -1
		});
		if (filter){
		  jQuery(element_to_annotate).annotator().annotator('addPlugin', 'Filter');
		}
		
		jQuery(element_to_annotate).annotator().annotator('addPlugin', 'Store', {
			// The endpoint of the store on your server.
			prefix: url,
			annotationData: {
				'bilag_id': bilag_id,
				'bullet_point_id': bullet_point_id,
				'meeting_id': meeting_id,
			},
			loadFromSearch: {
				'bilag_id': bilag_id,
				'bullet_point_id': bullet_point_id,
				'meeting_id': meeting_id,
			},
			urls: {
			  create:  'annotator/create',
			  read:    'annotator/read/:id',
			  update:  'annotator/update/:id',
			  destroy: 'annotator/delete/:id',
			  search:  'annotator/search'
			}
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
