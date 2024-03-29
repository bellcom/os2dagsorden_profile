/** global: Drupal */
/** global: DocumentTouch */
/** global: pageScroller */
/** global: screen */

/**
 * Hides print buttons for the iPad
 */
function hide_print_buttons(){
  jQuery(document).ready(function() {
     if (isTouchDevice()){
	jQuery(".print-button").hide();
     }
  });
}

/*
 * Adds the subscribe/unsubscribe behavior for elements in the follows committees section
 */
function follows_subscribe_click(event) {
    var subscribed_elements = [];
    if (jQuery(event.target).hasClass('premeeting')) {
      return;
    }
    if (jQuery(event.target).hasClass('committee_admin')) {
      return;
    }
    if (jQuery('#follows_subscribed_hidden').val() != '') {
        subscribed_elements = jQuery('#follows_subscribed_hidden').val().split(',');
    }

    var id_toggle = jQuery(event.target).attr('id').replace("follows_checkbox_", "");
    if (subscribed_elements.indexOf(id_toggle) > -1) {
        //removing element 1 element
        subscribed_elements.splice(subscribed_elements.indexOf(id_toggle), 1);
    } else {
        //adding this element
        subscribed_elements.push(id_toggle);
    }

    jQuery(event.target).toggleClass('subscribed');
    jQuery('#follows_subscribed_hidden').val(subscribed_elements.join(','));
    //console.log(event.target);
}

function premeeting_committee_click(event) {
  var premeeting_committee_elements = [];
  if (jQuery(event.target).hasClass('checkbox_follows')) {
    return;
  }
  if (jQuery(event.target).hasClass('committee_admin')) {
    return;
  }
  if (jQuery('#premeeting_committee_hidden').val() != '') {
    premeeting_committee_elements = jQuery('#premeeting_committee_hidden').val().split(',');
  }

  var id_toggle = jQuery(event.target).attr('id').replace("premeeting_checkbox_", "");
  if (premeeting_committee_elements.indexOf(id_toggle) > -1) {
    //removing element 1 element
    premeeting_committee_elements.splice(premeeting_committee_elements.indexOf(id_toggle), 1);
  } else {
    //adding this element
    premeeting_committee_elements.push(id_toggle);
  }

  jQuery('#premeeting_committee_hidden').val(premeeting_committee_elements.join(','));
    //console.log(event.target);
}

/*
 * Adds the subscribe/unsubscribe behavior for elements in the follows committees section
 */
function invisible_participants_click(event) {
    var invisible_elements = [];
    if (jQuery('#participants_invisible_hidden').val() != '') {
        invisible_elements = jQuery('#participants_invisible_hidden').val().split(',');
    }
     var id_toggle = jQuery(event.target).attr('id').replace("checkbox_", "");
    if (invisible_elements.indexOf(id_toggle) > -1) {
        //removing element 1 element
        invisible_elements.splice(invisible_elements.indexOf(id_toggle), 1);
    } else {
        //adding this element
        invisible_elements.push(id_toggle);
    }

     jQuery('#participants_invisible_hidden').val(invisible_elements.join(','));
    //console.log(event.target);
}

function committee_admin_click(event) {
  var premeeting_committee_elements = [];
  if (jQuery(event.target).hasClass('checkbox_follows')) {
    return;
  }
  if (jQuery(event.target).hasClass('premeeting')) {
   return;
  }
  if (jQuery('#committee_admin_hidden').val() != '') {
    premeeting_committee_elements = jQuery('#committee_admin_hidden').val().split(',');
  }
console.log('here');
  var id_toggle = jQuery(event.target).attr('id').replace("committee_admin_checkbox_", "");
  if (premeeting_committee_elements.indexOf(id_toggle) > -1) {
    //removing element 1 element
    premeeting_committee_elements.splice(premeeting_committee_elements.indexOf(id_toggle), 1);
  } else {
    //adding this element
    premeeting_committee_elements.push(id_toggle);
  }

  jQuery('#committee_admin_hidden').val(premeeting_committee_elements.join(','));
    //console.log(event.target);
}

jQuery(document).ready(function() {
    jQuery("ul.droptrue").sortable({
        connectWith: 'ul',
    });
    jQuery("ul.droptrue").bind("sortreceive", function(event, ui) {
        var arr_receiver = [];
        var arr_sender = [];
        jQuery(jQuery(this).children('li')).each(function(){
            arr_receiver.push(jQuery(this).attr('id'));
        });
        jQuery(jQuery(ui.sender[0]).children('li')).each(function(){
            arr_sender.push(jQuery(this).attr('id'));
        });
        jQuery('#' + jQuery(this).attr('id') + '_hidden').val(arr_receiver.join(','));
        jQuery('#' + jQuery(ui.sender[0]).attr('id') + '_hidden').val(arr_sender.join(','));
    });
    jQuery(".select-participants ul.droptrue li input[type=checkbox]").click(invisible_participants_click);
    jQuery(".available_users ul.droptrue li input[type=checkbox]").click(invisible_participants_click);
    //add click behaviour to existing items

    jQuery(".select-committee #edit-follows-div ul.droptrue li input.checkbox_follows").click(follows_subscribe_click);
    jQuery(".select-committee  ul.droptrue li span.checkbox.premeeting input").click(premeeting_committee_click);
    jQuery(".select-committee  ul.droptrue li span.checkbox.committee-admin input").click(committee_admin_click);
    //add behaviour to item that is added to the participants section
    jQuery(".select-participants ul.droptrue").bind("sortreceive", function(event, ui){
      jQuery('#follows_checkbox_' + ui.item.context.id).addClass('invisible');
    });
      //remove behaviour from item that is removed from the participants section
    jQuery(".select-participants ul.droptrue").bind("sortremove", function(event, ui){
         jQuery('#follows_checkbox_' + ui.item.context.id ).removeClass('invisible');
        jQuery(ui.item).find("input[type=checkbox]").removeAttr("checked");
      //removing this user from invisible
        var invisible_elements = [];
        if (jQuery('#participants_invisible_hidden').val() != '') {
            invisible_elements = jQuery('#participants_invisible_hidden').val().trim().split(',');
        }
        var id_toggle = jQuery(ui.item).attr('id');

        if (invisible_elements.indexOf(id_toggle) > -1) {
            //removing element 1 element
            invisible_elements.splice(invisible_elements.indexOf(id_toggle), 1);
        }
        jQuery('#participants_invisible_hidden').val(invisible_elements.join(','));
    });

    //add behaviour to item that is added to the follow section
    jQuery(".select-committee #edit-follows-div ul.droptrue").bind("sortreceive", function(event, ui){
       // jQuery(ui.item).find('span').addClass('can-subscribe');
       jQuery('#premeeting_checkbox_' + ui.item.context.id).addClass('premeeting');
       jQuery('#follows_checkbox_' + ui.item.context.id).addClass('checkbox_follows');
       jQuery('#follows_checkbox_' + ui.item.context.id).parent('span').addClass('can-subscribe');
       jQuery('#premeeting_checkbox_' + ui.item.context.id).addClass('premeeting');
       jQuery('#committee_admin_checkbox_' + ui.item.context.id).addClass('committee-admin');
        //console.log(ui.item);
        setTimeout(function(){
          jQuery(ui.item).find("input.checkbox_follows").bind('click', follows_subscribe_click);
          jQuery(ui.item).find(["input.checkbox.premeeting"]).bind('click', premeeting_committee_click); 
          jQuery(ui.item).find(["input.checkbox.committee-admin"]).bind('click',  committee_admin_click); }, 1);
    });
    jQuery(".select-committee ul.droptrue").bind("sortreceive", function(event, ui){
        jQuery('#premeeting_checkbox_' + ui.item.context.id).parent('span').addClass('premeeting');
        jQuery('#committee_admin_checkbox_' + ui.item.context.id).parent('span').addClass('committee-admin');

        //console.log(ui.item);
        setTimeout(function(){;
          jQuery(ui.item).find("#premeeting_checkbox_" + ui.item.context.id).bind('click', premeeting_committee_click);
          jQuery(ui.item).find("#committee_admin_checkbox_" + ui.item.context.id).bind('click',  committee_admin_click); }, 1);

    });
    //remove behaviour from item that is removed from the follow section
    jQuery(".select-committee.single ul.droptrue").bind("sortremove", function(event, ui){
        //jQuery(ui.item).removeClass('can-subscribe subscribed');
        jQuery('#follows_checkbox_' + ui.item.context.id ).removeClass('follows subscribed');
        jQuery('#follows_checkbox_' + ui.item.context.id ).removeClass('checkbox_follows');
        jQuery('#premeeting_checkbox_' + ui.item.context.id ).parent('span').removeClass('premeeting');
        jQuery('#committee_admin_checkbox_' + ui.item.context.id ).parent('span').removeClass('committee-admin');
        jQuery('#follows_checkbox_' + ui.item.context.id).parent('span').removeClass('can-subscribe');
        jQuery('#follows_checkbox_' + ui.item.context.id).parent('span').removeClass('subscribed');
        jQuery('#follows_checkbox_' + ui.item.context.id).addClass('checkbox');
        jQuery(ui.item).find("input[type=checkbox]").removeAttr("checked");
        jQuery('#follows_checkbox_' + ui.item.context.id).unbind('click');
        jQuery('#premeeting_checkbox_' + ui.item.context.id).unbind('click');
        jQuery('#committee_admin_checkbox' + ui.item.context.id).unbind('click');
        //removing this element from subscribed
        var subscribed_elements = [];
        if (jQuery('#follows_subscribed_hidden').val() != '') {
            subscribed_elements = jQuery('#follows_subscribed_hidden').val().trim().split(',');
        }
        var id_toggle = jQuery(ui.item).attr('id');

        if (subscribed_elements.indexOf(id_toggle) > -1) {
            //removing element 1 element
            subscribed_elements.splice(subscribed_elements.indexOf(id_toggle), 1);
        }
        jQuery('#follows_subscribed_hidden').val(subscribed_elements.join(','));

        //removing this element from premeeting
        var premeeting_committee_elements = [];
        if (jQuery('#premeeting_committee_hidden').val() != '') {
            premeeting_committee_elements = jQuery('#premeeting_committee_hidden').val().trim().split(',');
        }
        var id_toggle = jQuery(ui.item).attr('id');

        if (premeeting_committee_elements.indexOf(id_toggle) > -1) {
            //removing element 1 element
            premeeting_committee_elements.splice(premeeting_committee_elements.indexOf(id_toggle), 1);
        }
        jQuery('#premeeting_committee_hidden').val(premeeting_committee_elements.join(','));
    });


    jQuery("#edit-available-committee").css('height',jQuery(".select-committee").height()+"px");
    jQuery(".remove-committee").css('width',jQuery(".select-committee").width()+"px");
    jQuery("#edit-available-users").css('height',"150px");
    if (jQuery(".select-participants").height()) {
      jQuery("#edit-available-users").css('height',jQuery(".select-participants").height()+"px");
    }
    jQuery('.page-user-reset #block-system-main #edit-pass-pass1').focus();
    //updatePostOrder();
});

jQuery(document).ajaxComplete(function() {
  jQuery("ul.droptrue").sortable({
    connectWith: 'ul',
  });
  jQuery("ul.droptrue").bind("sortreceive", function(event, ui) {
    var arr_receiver = [];
    var arr_sender = [];
    jQuery(jQuery(this).children('li')).each(function(){
      arr_receiver.push(jQuery(this).attr('id'));
    });
    jQuery(jQuery(ui.sender[0]).children('li')).each(function(){
      arr_sender.push(jQuery(this).attr('id'));
    });
    jQuery('#' + jQuery(this).attr('id') + '_hidden').val(arr_receiver.join(','));
    jQuery('#' + jQuery(ui.sender[0]).attr('id') + '_hidden').val(arr_sender.join(','));
  });
});

jQuery(document).ready(function() {
  jQuery('#users_filter').change( function () {
    var filter = jQuery(this).val(); // get the value of the input, which we filter on
    jQuery('ul#available_users li').each(function() {
    if(jQuery(this).html().toUpperCase().indexOf(filter.toUpperCase()) > -1) {
      jQuery(this).show();
    }
    else {
      jQuery(this).hide();
    }
    })
  }).keyup( function () {
    // fire the above change event after every letter
    jQuery(this).change();
  });
});

jQuery(document).ready(function() {
  jQuery('#expand_all_agendas').click( function(e){
	e.preventDefault();
  if (jQuery('#expand_all_agendas').hasClass('expanded')) {
     jQuery(".ul-item-list-dagsordenspunkt").each(function(index) {

      jQuery("#attachments_container_" + index).hide();
      jQuery("#btn_hide_show_attachments_" + index).val("⇓");
      jQuery("#btn_hide_show_attachments_" + index).removeClass('opened');
      jQuery("[id^=attachment_text_container_" + index + "_]").each(function (index_attachment) {
      attachment_load_content(index, index_attachment, '/');
      jQuery("#btn_hide_show_attachment_text_" + index + "_" + index_attachment).val("⇓");
      jQuery("#btn_hide_show_attachment_text_" + index + "_" + index_attachment).text("⇓");
      jQuery("#btn_hide_show_attachment_text_" + index + "_" + index_attachment).removeClass('opened');
      jQuery(".btn_hide_show_all_attachments_text_" + index).removeClass('opened');
      jQuery(".btn_hide_show_all_attachments_text_" + index).val('⇊');
      jQuery(".btn_hide_show_all_attachments_text_" + index).text('⇊');

      jQuery("#btn_hide_show_attachments_" + index + "_" + index_attachment).removeClass('opened');

      jQuery(this).hide();

    });
  });
  } else {
    jQuery(".ul-item-list-dagsordenspunkt").each(function(index) {

      jQuery("#attachments_container_" + index).show();
      jQuery("#btn_hide_show_attachments_" + index).val("⇑");
      jQuery("#btn_hide_show_attachments_" + index).addClass('opened');
      jQuery("[id^=attachment_text_container_" + index + "_]").each(function (index_attachment) {
      attachment_load_content(index, index_attachment, '/');
      jQuery("#btn_hide_show_attachment_text_" + index + "_" + index_attachment).val("⇑");
      jQuery("#btn_hide_show_attachment_text_" + index + "_" + index_attachment).text("⇑");
      jQuery("#btn_hide_show_attachment_text_" + index + "_" + index_attachment).addClass('opened');
      jQuery(".btn_hide_show_all_attachments_text_" + index).addClass('opened');
      jQuery(".btn_hide_show_all_attachments_text_" + index).val('⇈');
      jQuery(".btn_hide_show_all_attachments_text_" + index).text('⇈');

      jQuery("#btn_hide_show_attachments_" + index + "_" + index_attachment).addClass('opened');

      jQuery(this).show();

    });

    jQuery(".btn_hide_show_all_attachments_text_" + index).val('⇈')
    });

    }
    jQuery('#expand_all_agendas').toggleClass('expanded');
  });

});

jQuery(document).ready(function() {
    jQuery('.form-item-from-date-value-date input.form-text').change(function(){
          jQuery(this).val(prepareDate(jQuery(this).val()));
    })
    jQuery('.form-item-to-date-value-date input.form-text').change(function(){
          jQuery(this).val(prepareDate(jQuery(this).val()));
    })
    // Set body font size.
    jQuery('body').css({'font-size' : Drupal.settings.os2dagsorden_settings.body_font_size+'px'});

    // Set title font size.
    jQuery('table.views-table tr td a').css({'font-size' : Drupal.settings.os2dagsorden_settings.title_font_size+'px'});
    jQuery('.view-user-committee .views-row').css({'font-size' : Drupal.settings.os2dagsorden_settings.title_font_size+'px'});
    jQuery('.view-user-follow-committees .views-row').css({'font-size' : Drupal.settings.os2dagsorden_settings.title_font_size+'px'});
    jQuery('.view-user-follow-committees .view-empty').css({'font-size' : Drupal.settings.os2dagsorden_settings.title_font_size+'px'});


    // Sidepane arrow position.
    set_up_button_position('.front #block-block-1', '.front #region-sidebar-second', 'front', jQuery(window).width());
    set_up_button_position('.not-front #block-block-1', '.not-front #region-sidebar-second', 'not-front', jQuery(window).width());
    resize_listener();
});
function set_up_button_position(button_id, region_sidebar_second, frontpage, width) {
  // Sidepane arrow position.
  if (Drupal.settings.os2dagsorden_settings.sidepane_arrow_position != 'classic' && width > 740) {
    if (frontpage == 'front') {
      jQuery(button_id).css({'top' : '73px'});
      jQuery(region_sidebar_second).css({'margin-top':'42px'});
    }
    else {
      jQuery(button_id).css({'top' : '66px'});
      jQuery(region_sidebar_second).css({'margin-top':'0px'});
      jQuery('#breadcrumb').css({'width': '80%'});
    }
  }
  else if (width < 740) {
    if (frontpage == 'front') {
      jQuery(button_id).css({'top' : '28px'});
      jQuery(region_sidebar_second).removeAttr("style");
    }

  }
}
/* Changed ddmmyy and ddmmyyyy date formats to dd-mm-yyyy  */
function prepareDate(dateValue) {
   var fromDateArray = dateValue.match( /^([0-9]{2})([0-9]{2})([0-9]{2,4})$/);
   if (fromDateArray){
    if (fromDateArray[3].length<4)
      fromDateArray[3]=2000 + parseInt(fromDateArray[3]);
       return fromDateArray[1]+'-'+fromDateArray[2]+'-'+fromDateArray[3];
   }
   else{
     return dateValue;
  }
 }
/**
 * Fixes the bug when two click are needed to follow link on iPad
 */
function add_indicator_help_text(){
  jQuery(document).ready(function() {
      jQuery('.indicator-has-notes').each(function() {
	  jQuery(this).attr('title', 'Ikonet er ikke klikbart, men angiver at du har lavet en eller flere noter.');
      });
      jQuery('.indicator-has-speaker-paper').each(function() {
	  jQuery(this).attr('title', 'Ikonet er ikke klikbart, men angiver at du har oprettet et talepapir.');
      });
  });
}


/**
 *Add the listener to the tabler orientation property. On device rotation side menu is either forces to be closed, or shown
 */
function add_tablet_orientation_listener(){
  jQuery(document).ready(function() {
      jQuery(window).bind('orientationchange', function() {
          switch (window.orientation) {
              case -90:
              case 90:
                  if (JSON.parse(window.localStorage.getItem('hide_side_menu')) === true) {
                      hide_side_menu(false);
                  } else {
                      show_side_menu(false);
                  }
                  break;
              default:
                  hide_side_menu(false);
                  break;
          }
     });
  });
}

/**
 * Adds the behaviour of showing/hidng the right side panel with menu.
 */

function add_show_hide_menu_behaviour(menu_collapse){
   jQuery(document).ready(function() {
      jQuery("#region-content").removeAttr("style");
       if (menu_collapse && (window.localStorage.getItem('hide_side_menu') === null || window.localStorage.getItem('hide_side_menu').length === 0)) {
          hide_side_menu(false);
       } else if (JSON.parse(window.localStorage.getItem('hide_side_menu')) === true) {
           hide_side_menu(false);
       }
       //forcing to be closed on portrain orientation
       if (window.orientation == 0 || window.orientation == 180) {
           hide_side_menu(false);
       }
       jQuery("#show_hide_menu_button").click(function(){
          //saving for landscape orientation only, portrait does not affect the saved value
          var saveForLandscapeOnly = (window.orientation != 0 && window.orientation != 180);
          if (jQuery("#show_hide_menu_button").val() == "⇒") {
              hide_side_menu(saveForLandscapeOnly);
          }
          else {
              show_side_menu(saveForLandscapeOnly);
          }
       });
   });
}

function resize_listener(){
  function decide_menu_visible() {
    var width = (window.innerWidth > 0) ? window.innerWidth : screen.width;
    // Sidepane arrow position.
    if (width < 740 || width > 740) {
      set_up_button_position('.front #block-block-1', '.front #region-sidebar-second', 'front', width);
      set_up_button_position('.not-front #block-block-1', '.not-front #region-sidebar-second', 'not-front', width);
    }
//    if (width > 768) {
//       if (JSON.parse(window.localStorage.getItem('hide_side_menu')) != true) {
//         show_side_menu(false);
//       }
//    }

  };

  var resizeTimer;
  jQuery(window).resize(function() {
    clearTimeout(resizeTimer);
    resizeTimer = setTimeout(decide_menu_visible, 100);
  });
}

/**
 * A funtion to hide the menu
 */
function hide_side_menu(user_defined){
  jQuery(document).ready(function() {
	jQuery(".region-sidebar-second-inner").hide();
	jQuery("#show_hide_menu_button").val("⇐");
  jQuery("#show_hide_menu_button").text("⇐");
	jQuery("#region-content").removeClass("grid-18");
	jQuery("#region-content").addClass("grid-24");
    if (Drupal.settings.os2dagsorden_settings.sidepane_arrow_position != 'classic') {
      if (jQuery(window).width() > 980 ) {
        jQuery("#region-content").css({'width': '93%'});
      }
      else if (jQuery(window).width() > 740 && jQuery(window).width() < 980 ) {
        jQuery("#region-content").css({'width': '91%'});
      }
      else {
        jQuery("#region-content").removeAttr("style");
      }
    }
    if (user_defined === true) {
      window.localStorage.setItem("hide_side_menu", "true");
    }
  });
}

/**
 * A funtion show hide the menu
 */
function show_side_menu(user_defined){
	jQuery(".region-sidebar-second-inner").show();
	jQuery("#show_hide_menu_button").val("⇒");
  jQuery("#show_hide_menu_button").text("⇒");
	jQuery("#region-content").removeClass("grid-24");
	jQuery("#region-content").addClass("grid-18");
    if (Drupal.settings.os2dagsorden_settings.sidepane_arrow_position != 'classic') {
      jQuery("#region-content").removeAttr("style");
    }
    if (user_defined === true) {
        window.localStorage.setItem("hide_side_menu", "false");
    }
}

/**
 * Adds the expand behaviour to bullet point on meeting or bullet-point view
 *
 * @url is base url, used to send the parameted to attachment_add_expand_behaviour()
 */
function bullet_point_add_expand_behaviour(url, massive_bilag_expand, bullet_points_expand, attachments_expand){
  var pathname = window.location.pathname;
   jQuery(document).ready(function() {
        jQuery(".bullet-point-attachments .view-content .item-list .ul-item-list-dagsordenspunkt").each(function(index) {
          jQuery(this).attr("id","attachments_container_"+index);
          jQuery(this).hide();

          jQuery(this).parent().parent().parent().children(".hide_show_button_container").append("<button class='button' id='btn_hide_show_attachments_"+index+"' value='⇓'>⇓</button>");

         jQuery("#btn_hide_show_attachments_"+index).click(function(){
             jQuery("#attachments_container_"+index).toggle();
             if (jQuery("#btn_hide_show_attachments_"+index).val() == "⇓"){//closed
                jQuery("#btn_hide_show_attachments_"+index).val("⇑");
                jQuery("#btn_hide_show_attachments_"+index).text("⇑");
                jQuery("#btn_hide_show_attachments_"+index).addClass('opened');
                if (attachments_expand)
                    bullet_points_expand_all(this, index, url, massive_bilag_expand, attachments_expand);
                //saving in local storage
                window.localStorage.setItem(pathname + "-attachments_container_"+index, "true");
            }
            else {//opened
                jQuery("#btn_hide_show_attachments_"+index).val("⇓");
                jQuery("#btn_hide_show_attachments_"+index).text("⇓");
                jQuery("#btn_hide_show_attachments_"+index).removeClass('opened');
                //saving in local storage
                window.localStorage.setItem(pathname + "-attachments_container_"+index, "false");
            }
           });

          attachment_add_expand_all_behaviour(this, index, url, massive_bilag_expand);
          attachment_add_expand_behaviour(this,index,url, massive_bilag_expand, attachments_expand);

         if (bullet_points_expand && (window.localStorage.getItem(pathname + "-attachments_container_"+index)===null||window.localStorage.getItem(pathname + "-attachments_container_"+index)===true)){
                  bullet_points_expand_all(this, index, url, massive_bilag_expand, attachments_expand);
          }
          else{     //reading from local storage
          if (JSON.parse(window.localStorage.getItem(pathname + "-attachments_container_"+index)) === true){
            jQuery("#btn_hide_show_attachments_"+index).click();
                        }
                }
        });
 });
}

/**
 * Initiator function to add expand behaviour for bullet point, is used on bullet-point view
 *
 * @url is base url, used to send the parameted to attachment_add_expand_behaviour()
 */
function bullet_point_details_init(url, massive_bilag_expand, attachments_expand){
  jQuery(document).ready(function() {
    jQuery(".item-list-dagsordenspunkt .ul-item-list-dagsordenspunkt").each(function(index) {
	attachment_add_expand_all_behaviour(this, index, url, massive_bilag_expand);
	attachment_add_expand_behaviour(this, index, url, massive_bilag_expand);
        bilag_cases_add_expand_behaviour(this, index);
       if (attachments_expand)
            bullet_points_expand_all(this, index, url, massive_bilag_expand, attachments_expand);
    });
  });
}

function bullet_points_expand_all(bulletPoint, bulletPointIndex, url, massive_bilag_expand, attachments_expand) {
    jQuery("#attachments_container_" + bulletPointIndex).show();
    jQuery("#btn_hide_show_attachments_" + bulletPointIndex).val("⇑");
    jQuery("#btn_hide_show_attachments_" + bulletPointIndex).addClass('opened');
    if (attachments_expand) {
        jQuery("[id^=attachment_text_container_" + bulletPointIndex + "_]").each(function (index_attachment) {
            attachment_load_content(bulletPointIndex, index_attachment, url);
            jQuery("#btn_hide_show_attachment_text_" + bulletPointIndex + "_" + index_attachment).val("⇑");
            jQuery("#btn_hide_show_attachments_" + bulletPointIndex).addClass('opened');
            jQuery(this).show();

        });
    }
    jQuery(".btn_hide_show_all_attachments_text_" + bulletPointIndex).val('⇈');
}
/**
 * Add expand all behavious for bullet point - opens all of its children.
 *
 * Also loads the comment of the attachment via Ajax and adds the annotator to it, if these actions has not been done before
 *
 */
function attachment_add_expand_all_behaviour(bulletPoint, bulletPointIndex, url, massive_bilag_expand){
  var pathname = window.location.pathname;

  if (jQuery('li', bulletPoint).size() > 1) {
      jQuery(bulletPoint).prepend("<button class='button hide_show_all_attachments_text btn_hide_show_all_attachments_text_"+bulletPointIndex+" top' value='⇊'>⇊</button>");
      jQuery(bulletPoint).append("<button class='button hide_show_all_attachments_text btn_hide_show_all_attachments_text_"+bulletPointIndex+" bottom' value='⇊'>⇊</button>");

      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).click(function(){
        if (jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).val() == "⇊"){
        jQuery("[id^=attachment_text_container_"+bulletPointIndex+"_]").each(function(index_attachment){
          if (massive_bilag_expand || !jQuery(this).children().first().hasClass("attachment_text_trimmed_container")){//skip bilags
            //saving in the local storage
            window.localStorage.setItem(pathname + "-attachment_text_container_"+bulletPointIndex+"_"+index_attachment, "true");
            jQuery(this).show();

            //handle single expand button
            jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).val("⇑");
            jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).addClass('opened');
          }

          attachment_load_content(bulletPointIndex, index_attachment, url);
        });

        jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).val("⇈");
        jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).text("⇈")
        jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).addClass('opened');
        } else {
            jQuery("[id^=attachment_text_container_"+bulletPointIndex+"_]").each(function(index_attachment){
          //saving in the local storage
          window.localStorage.setItem(pathname + "-attachment_text_container_"+bulletPointIndex+"_"+index_attachment, "false");
          jQuery(this).hide();

          //handle single expand button
          jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).val("⇓");
          jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).text("⇓");
          jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).removeClass('opened');
        });

        jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).val("⇊");
        jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).text("⇊")
        jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).removeClass('opened');
        }
      });
  }
}

/**
 * Adds expand behaviour on a single attachment.
 *
 * Also calls attachment_load_content
 */
function attachment_add_expand_behaviour(bulletPoint, bulletPointIndex, url, massive_bilag_expand,  attachments_expand){
  var pathname = window.location.pathname;

  jQuery(bulletPoint).children("li").children(".attachment_text_container").each(function(index_attachment){
    jQuery(this).attr("id","attachment_text_container_"+bulletPointIndex+"_"+index_attachment);
    jQuery(this).hide();

    jQuery(this).parent().prepend("<button class='button hide_show_attachment_text' id='btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment+"' value='⇓'>⇓</button>");
    jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).click(function(){
      //hide or show the content container
      jQuery("#attachment_text_container_"+bulletPointIndex+"_"+index_attachment).toggle();

      attachment_load_content(bulletPointIndex, index_attachment, url);

      //change the arrow button icon
      if (jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).val() == "⇓"){//closed
	jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).val("⇑");
  jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).text("⇑");
  jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).addClass('opened');
	//saving in local storage
	window.localStorage.setItem(pathname + "-attachment_text_container_"+bulletPointIndex+"_"+index_attachment, "true");
      }
      else {//opened
	jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).val("⇓");
  jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).text("⇓");
  jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).removeClass('opened');
	//saving in local storage
	window.localStorage.setItem(pathname + "-attachment_text_container_"+bulletPointIndex+"_"+index_attachment, "false");
      }

      //handle expand all
      if (attachments_expand){
	if (jQuery("[id^=btn_hide_show_attachment_text_"+bulletPointIndex+"_][value='⇓']").length > 0)
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).val("⇊");
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).text("⇊");
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).removeClass('opened');
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).val("⇈");
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).text("⇈");
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).addClass('opened')
      } else {
	var new_val = "⇈";
	jQuery("[id^=btn_hide_show_attachment_text_"+bulletPointIndex+"_]").each(function(){
	  if (jQuery(this).parent().hasClass("non-bilag")){
	    if (jQuery(this).val() == '⇓'){
	      new_val = "⇊";
	    }
	  }
	});
    jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).val(new_val);
    jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).text(new_val);
    if(new_val == "⇊") {
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).removeClass('opened');
    }
    else {
      jQuery(".btn_hide_show_all_attachments_text_"+bulletPointIndex).addClass('opened');
    }

      }
    });

    //reading from local storage
    if (JSON.parse(window.localStorage.getItem(pathname + "-attachment_text_container_"+bulletPointIndex+"_"+index_attachment)) === true){
      jQuery("#btn_hide_show_attachment_text_"+bulletPointIndex+"_"+index_attachment).click();
    }
  });
  jQuery("#attachments_container_"+bulletPointIndex).each(function() {
      jQuery(this).children("li.bilags_cases").children(".bilags_cases_container").attr("id","bilags_cases_container_"+bulletPointIndex);
      jQuery(this).children("li.bilags_cases").children(".bilags_cases_container").hide();
      bilag_cases_add_expand_behaviour(this,bulletPointIndex);
    });
  }

/**
 * Loads the content of the attachment and places it into the container
 *
 * Also loads the comment of the attachment via Ajax and adds the annotator to it, if these actions has not been done before
 */
function attachment_load_content(bulletPointIndex, index_attachment, url){
    //load the content on first click and add the annotator
    if (jQuery("#attachment_text_container_"+bulletPointIndex+"_"+index_attachment).children().contents().first().text() == "Vent venligst..."){
      //get meeting id, bullet-point id and bilag id
      var classes = jQuery("#attachment_text_container_"+bulletPointIndex+"_"+index_attachment).children().attr('class').split(' ');
      var cl = jQuery.grep(classes, function(string){
	    return (string.indexOf("bpa-") == 0);
      });

      var cl_arr = String(cl).split("-");
      var bilag_id = cl_arr[3];
      var bullet_point_id = cl_arr[2];
      var meeting_id = cl_arr[1];

      //add real content
      jQuery.get(url + "meeting/" + meeting_id + "/bullet-point/" + bullet_point_id + "/bullet-point-attachment-raw/" + bilag_id, function(html) {
	//remove dummy text
	jQuery("#attachment_text_container_"+bulletPointIndex+"_"+index_attachment).children().contents().first().remove();
	jQuery("#attachment_text_container_"+bulletPointIndex+"_"+index_attachment).children().contents().first().remove();

	jQuery("#attachment_text_container_"+bulletPointIndex+"_"+index_attachment).children().first().append(html);

	//add annotator to it
	add_annotator(meeting_id, bullet_point_id, bilag_id, "bpa_"+ meeting_id + "_" + bullet_point_id + "_" + bilag_id ,url, false);

	//add preview stamp picture, if is actual bilag
	if (jQuery(".bpa-" + meeting_id + "-" + bullet_point_id + "-" + bilag_id).hasClass("attachment_text_trimmed_container"))
	  jQuery(".bpa-" + meeting_id + "-" + bullet_point_id + "-" + bilag_id).prepend('<div class="indicator-preview"></div>');
      });
    }
}
function bilag_cases_add_expand_behaviour(bulletPoint, bulletPointIndex){
  var pathname = window.location.pathname;

  jQuery(bulletPoint).children("li").children(".bilags_cases_container").each(function(index_attachment){
    jQuery(this).attr("id","bilags_cases_container_"+bulletPointIndex+"_"+index_attachment);
    jQuery(this).hide();
    jQuery(this).parent().prepend("<button class='button hide_show_bilags_cases' id='btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment+"' value='⇓'>⇓</button>");
    jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).click(function(){
      //hide or show the content container
      jQuery("#bilags_cases_container_"+bulletPointIndex+"_"+index_attachment).toggle();

      //change the arrow button icon
      if (jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).val() == "⇓"){//closed
	jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).val("⇑");
  jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).text("⇑");
  jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).addClass('opened');
	//saving in local storage
	window.localStorage.setItem(pathname + "-bilags_cases_container_"+bulletPointIndex+"_"+index_attachment, "true");
      }
      else {//opened
	jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).val("⇓");
  jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).text("⇓");
  jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).removeClass('opened');
	//saving in local storage
	window.localStorage.setItem(pathname + "-bilags_cases_container_"+bulletPointIndex+"_"+index_attachment, "false");
      }

    });
    //reading from local storage
    if (JSON.parse(window.localStorage.getItem(pathname + "-bilags_cases_container_"+bulletPointIndex+"_"+index_attachment)) === true){
      jQuery("#btn_hide_bilags_cases_"+bulletPointIndex+"_"+index_attachment).click();
    }
  });
}

/**
 * Adds notes indicators, to bullet point attachment
 *
 */
function bullet_point_attachment_add_notes_indicator(ids){
  jQuery(document).ready(function() {
	jQuery(".indicator-has-no-notes").each(function(){
	  if (ids.indexOf(parseInt(this.id)) != -1){
	    jQuery(this).attr("class","indicator-has-notes");
	  }
	});
   });
}

/**
 * Checks if device is touchable
 *
 */
function isTouchDevice(){
  return "ontouchstart" in window || window.DocumentTouch && document instanceof DocumentTouch;
}

function addPagescroller(){
  if (isTouchDevice()){
    jQuery(document).ready(function() {

      var maxPages = jQuery('.views-field-php-4').size();

	    if (maxPages > 1){//meeting is not empty
        jQuery('<div class="section section-bottom"></div>').insertAfter(jQuery('#section-content'));

	      jQuery('#page').pageScroller({
		      navigation: '#arrow-controls',
		      sectionClass: 'section',
	      });

	      jQuery('.bullet-point-attachments').prepend('<div id="arrow-controls" class="light right">'
	      + '<a href="#" class="prev"></a><br/>'
	      + '<a href="#" class="next"></a>'
	      + '</div>');

	      // assigns 'next' API command to link
	      jQuery('#arrow-controls .next').bind('click', function(e){
		      e.preventDefault();
          //console.log(pageScroller.current);
		      //pageScroller.goTo(page);
              pageScroller.next();
	      });

  	    // assigns 'previous' API command to link
  	    jQuery('#arrow-controls .prev').bind('click', function(e){
  		    e.preventDefault();

  		    //console.log(pageScroller.current);
  		    //pageScroller.goTo(page);
  		    pageScroller.prev();

  	    });
	    }
    });
  }
}

function hide_budget_menu(){
  jQuery(document).ready(function() {
      jQuery("#menu-budget").parent().hide();
  });
}

function hide_massive_expand_collapse_button(){
  jQuery(document).ready(function() {
    jQuery(".ul-item-list-dagsordenspunkt").each(function(index) {
          //jQuery("#btn_hide_show_all_attachments_text_"+index).hide();
          jQuery(".btn_hide_show_all_attachments_text_"+index).hide();
    });
  });
}

function hide_search_block_title(){
 jQuery(document).ready(function() {
   jQuery("#block-views-exp-meetings-search-page .block-title").hide();
  });
}

function open_all_bilag_case_bullet_points(expand_bilags, expand_cases) {
  jQuery(document).ready(function() {
   if (expand_bilags)  {
    jQuery("li.bilags").children(".hide_show_bilags_cases").each(function() {
       if (jQuery(this).val() == '⇓') {

	jQuery(this).click();
      }
    });
  }
  if(expand_cases)  {
    jQuery("li.cases").children(".hide_show_bilags_cases").each(function() {
      if (jQuery(this).val() == '⇓') {
	jQuery(this).click();
      }
    });
  }
  });
}
  jQuery(document).ready(function() {
    jQuery("body").append("<div id='ToolTipDiv' class='tip-darkgray'></div>");
    jQuery("body").append("<div id='ToolTipDiv2' class='tip-darkgray'></div>");
    jQuery("div.help-button").each(function() {

      jQuery(this).click(function() {
        var offset = jQuery(this).offset();
       // alert(jQuery(this).attr('aria-label'));
        if (jQuery("#ToolTipDiv").css('display') == 'block') {
          jQuery("#ToolTipDiv").css({'position': 'absolute', 'bottom': '', 'right': ''});
          if (jQuery("#ToolTipDiv").hasClass(jQuery(this).attr('id'))) {
            jQuery("#ToolTipDiv").fadeOut(400);
            jQuery("#ToolTipDiv").attr('class', 'tip-darkgray');
          }
          else {
            jQuery("#ToolTipDiv").css({'top': offset.top + 30, 'left': offset.left - 300, 'max-width': '300px'});
            jQuery("#ToolTipDiv").attr('class', 'tip-darkgray');
            jQuery("#ToolTipDiv").addClass(jQuery(this).attr('id'));
            jQuery("#ToolTipDiv").html(jQuery(this).attr('aria-label')).fadeIn(400);
          }
        }
        else {
          jQuery("#ToolTipDiv").css({'top': offset.top + 30, 'left': offset.left - 300, 'max-width': '300px'});
          jQuery("#ToolTipDiv").addClass(jQuery(this).attr('id'));
          jQuery("#ToolTipDiv").html(jQuery(this).attr('aria-label')).fadeIn(400);

        }

      });
    });
  });
jQuery(document).ready(function() {
  jQuery("#zoom_in_button").click(function(){
    if(navigator.userAgent.indexOf("Chrome") != -1 ){
      var currentZoom = jQuery('body').css('zoom');
      var newZoom = parseFloat(currentZoom)  + 0.1;
      if (newZoom > 5) return;
      jQuery('body').css('zoom', newZoom);
    }
    else if(navigator.userAgent.indexOf("Safari") != -1){
      var currentZoom = jQuery('body').css('zoom');
      var newZoom = parseFloat(currentZoom)  + 0.1;
      if (newZoom > 5) return;
      jQuery('body').css('zoom', newZoom);
    }
    else if(navigator.userAgent.indexOf("Firefox") != -1 ){
      if (jQuery('body').css('-moz-transform') == 'none' || !jQuery('body').css('-moz-transform')) {
        currentFFZoom  = 1;
      }
      else {
        currentFFZoom  = jQuery('body').css('-moz-transform').match(/-?[\d\.]+/g)[0];
      }
      var newFFZoom = parseFloat(currentFFZoom)  + 0.1;
      if (newFFZoom > 5) return;
      jQuery('body').css('-moz-transform', 'scale(' +  newFFZoom + ')');
    }
    else if((navigator.userAgent.indexOf("MSIE") != -1 ) || (!!document.documentMode == true )){
      if (jQuery('body').css('zoom') == 'normal') {
        currentIEZoom  = '100%';
      }
      else {
        currentIEZoom  = jQuery('body').css('zoom');
      }

      newIEZoom = currentIEZoom.replace("%", "");
      newIEZoom = parseFloat(newIEZoom) + 10;
      if (newIEZoom > 500) return;
      jQuery('body').css('zoom', newIEZoom +'%');
    }
  });
  jQuery("#zoom_out_button").click(function(){
    if(navigator.userAgent.indexOf("Chrome") != -1 ){
      var currentZoom = jQuery('body').css('zoom');
      var newZoom = parseFloat(currentZoom)  - 0.1;
      if (newZoom < 1) return;
      jQuery('body').css('zoom', newZoom);
    }
    else if(navigator.userAgent.indexOf("Safari") != -1){
      var currentZoom = jQuery('body').css('zoom');
      var newZoom = parseFloat(currentZoom)  - 0.1;
      if (newZoom < 1) return;
      jQuery('body').css('zoom', newZoom);
    }
    else if(navigator.userAgent.indexOf("Firefox") != -1 ){
      if (jQuery('body').css('-moz-transform') == 'none' || !jQuery('body').css('-moz-transform')) {
        currentFFZoom  = 1;
      }
      else {
        currentFFZoom  = jQuery('body').css('-moz-transform').match(/-?[\d\.]+/g)[0];
      }
      var newFFZoom = parseFloat(currentFFZoom)  - 0.1;
      if (newFFZoom < 1) return;
      jQuery('body').css('-moz-transform', 'scale(' +  newFFZoom + ')');
    }
    else if((navigator.userAgent.indexOf("MSIE") != -1 ) || (!!document.documentMode == true )){
      if (jQuery('body').css('zoom') == 'normal') {
        currentIEZoom  = '100%';
      }
      else {
        currentIEZoom  = jQuery('body').css('zoom');
      }
      newIEZoom = currentIEZoom.replace("%", "");
      newIEZoom = parseFloat(newIEZoom) - 10;
      if (newIEZoom <= 100) return;
      jQuery('body').css('zoom', newIEZoom +'%');
    }
  });
jQuery("#zoom_reset_button").click(function(){
    if(navigator.userAgent.indexOf("Chrome") != -1 ){
      jQuery('body').css('zoom', 1);
    }
    else if(navigator.userAgent.indexOf("Safari") != -1){
      jQuery('body').css('zoom', 1);
    }
    else if(navigator.userAgent.indexOf("Firefox") != -1 ){
      jQuery('body').css('-moz-transform', 'scale(1)');
    }
    else if((navigator.userAgent.indexOf("MSIE") != -1 ) || (!!document.documentMode == true )){
      jQuery('body').css('zoom', '100%');
    }
  });
  jQuery('#views-exposed-form-meetings-search-page input#edit-reset').click(function(event){
    event.preventDefault();
    jQuery(this.form).clearForm();
    jQuery('#edit-from-date-value-datepicker-popup-0').val(Drupal.settings.os2dagsorden_settings.search_startdate);
    jQuery('#edit-to-date-value-datepicker-popup-0').val(Drupal.settings.os2dagsorden_settings.search_enddate)
    jQuery('#edit-from-date-value-datepicker-popup-1').val(Drupal.settings.os2dagsorden_settings.search_startdate);
    jQuery('#edit-to-date-value-datepicker-popup-1').val(Drupal.settings.os2dagsorden_settings.search_enddate)

    jQuery('#edit-field-os2web-meetings-committee-tid-depth').val('All');
  });


});

// Re-allocate td.
(function() {

  // Wait until the DOM has loaded.
  document.addEventListener('DOMContentLoaded', function() {
    var pageWrapper = document.querySelector('.page-meeting');
    var listElements = document.querySelectorAll('.bullet-point-attachments .item-list');
    var elements = [];
    var totalWidth = 0;

    // Only do stuff if we are on a meetings page.
    if (pageWrapper !== null) {

      // Bullet point modify links.
      var bulletPointModifyContainers = document.querySelectorAll('.bullet-point-modify-links-container');
      if (bulletPointModifyContainers.length > 0) {
        elements.push(bulletPointModifyContainers[0]);
      }

      // Speaker paper container.
      var speakerPaperContainers = document.querySelectorAll('.speaker-paper-link-container');
      if (speakerPaperContainers.length > 0) {
        elements.push(speakerPaperContainers[0]);
      }

      // Print bullet point.
      var printBulletPointContainers = document.querySelectorAll('.print-bullet-point-link');
      if (printBulletPointContainers.length > 0) {
        elements.push(printBulletPointContainers[0]);
      }

      // Send bullet point.
      var sendBulletPointContainers = document.querySelectorAll('.send-bullet-point-link');
      if (sendBulletPointContainers.length > 0) {
        elements.push(sendBulletPointContainers[0]);
      }

      // Case ID.
      var caseIdContainers = document.querySelectorAll('.case-id-link-container');
      if (caseIdContainers.length > 0) {
        elements.push(caseIdContainers[0]);
      }

      // Run through all elements.
      for (var i = 0; i < elements.length; i++) {
        var element = elements[i];

        totalWidth += element.offsetWidth;
      }

      // Run through all list elements.
      for (var i = 0; i < listElements.length; i++) {
        var listElement = listElements[i];

        listElement.style.marginRight = 0 - totalWidth + 'px';
      }
    }
  });
})();
