(function ($) {
  Drupal.behaviors.confirm = {
    attach: function (context, settings) {
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
      $("#js-bpas-fs .fieldset-wrapper").sortable({
        update: function (event, ui) {
          $.post(
            '/agenda/meeting/bullet-point/sort',
            {
              bullet_point: $("#js-bpas-fs").data("bp_id"),
              attachments: $('#js-bpas-fs .fieldset-wrapper').sortable('serialize')
            }
          );
        }
      });
    }
  }
})(jQuery);