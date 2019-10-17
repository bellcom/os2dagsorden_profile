(function ($) {
  Drupal.behaviors.confirm = {
    attach: function (context, settings) {
      if (Drupal.ajax) {
        Drupal.ajax.prototype.commands.closeLightboxPopup = function(ajax, response, status){
        parent.location.reload();
        parent.Lightbox.end();
      }
       Drupal.ajax.prototype.commands.closeLightboxAndRedirect = function(ajax, response, status){
        parent.location.href = response.redirect;
        parent.Lightbox.end();
      }
    }
    $('#edit-start-date-datepicker-popup-0').datepicker({
        onClose: function(){
          $(this).focus();
          $('#edit-end-date-datepicker-popup-0').val( $(this).val())
        }
    });

    $('#edit-end-date-datepicker-popup-0').datepicker({
        onClose: function(){
        $(this).focus();
        }
    });

    $('#edit-start-date-timeEntry-popup-1').change(
        function(){
          $('#edit-end-date-timeEntry-popup-1').val($(this).val());
          $(this).focus();
        }
    );
    $('#edit-end-date-timeEntry-popup-1').change(
        function(){
          $('#edit-end-date-timeEntry-popup-1').val($(this).val());
          $(this).focus();
        }
    );
      $('.create-agenda-edit-delete-container.form-submit-delete').once('form-submit-delete-processed').each(function (index) {
        var events = $(this).clone(true).data('events');// Get the jQuery events.
        $(this).unbind('mousedown'); // Remove the click events.
        $(this).mousedown(function () {
          if (confirm(Drupal.t('Are you sure you want to delete this item?'))) {
            $.each(events.mousedown, function () {
              this.handler(); // Invoke the mousedown handlers that was removed.
            });
          }

          // Prevent default action.
          return false;
        })
      });
    $('#edit-start-date-datepicker-popup-0').datepicker({
        onClose: function(){
        $(this).focus();
      }
    });

    $('#edit-end-date-datepicker-popup-0').datepicker({
        onClose: function(){
        $(this).focus();
        }
    });
      jQuery('.edit-meeting-btn.form-submit-delete').click(function (event) {
        event.preventDefault();
        if (confirm(Drupal.t('Are you sure you want to delete this item?'))) {
          var element = jQuery(this);
          jQuery.get(jQuery(this).attr('href'), function (data) {
            if (data.status == 'OK') {
              if (data.element == 'bp') {
                element.parents('tr').remove();
              } else if (data.element == 'bpa') {
                element.parents('li').remove();
              }
            }

          });
        }
      });
      $("#js-bpas-fs #js-bpa-non-bilags").sortable({
        update: function (event, ui) {
          $.post(
            '/agenda/meeting/bullet-point/sort',
            {
              bpa_type: 'non-bilags',
              bullet_point: $("#js-bpas-fs").data("bp_id"),
              attachments: $('#js-bpas-fs #js-bpa-non-bilags').sortable('serialize'),
            }
          );
        }
      });
      $("#js-bpas-fs #js-bpa-bilags").sortable({
        update: function (event, ui) {
          $.post(
            '/agenda/meeting/bullet-point/sort',
            {
              bpa_type: 'bilags',
              bullet_point: $("#js-bpas-fs").data("bp_id"),
              attachments: $('#js-bpas-fs #js-bpa-bilags').sortable('serialize')
            }
          );
        }
      });
       $("#js-agenda-fs .fieldset-wrapper").sortable({
        update: function (event, ui) {
          $.post(
            '/agenda/meeting/sort',
            {
              meeting_id: $("#js-agenda-fs").data("meeting_id"),
              attachments: $('#js-agenda-fs .fieldset-wrapper').sortable('serialize')
            }
          );
        }
      });
    }
  }
})(jQuery);
