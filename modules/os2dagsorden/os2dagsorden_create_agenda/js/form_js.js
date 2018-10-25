(function ($) {
    Drupal.behaviors.confirm = {
        attach: function (context, settings) {
            $('.form-submit-delete').once('form-submit-delete-processed').each(function(index) {
                var events = $(this).clone(true).data('events');// Get the jQuery events.
                $(this).unbind('mousedown'); // Remove the click events.
                $(this).mousedown(function() {
                    if (confirm(Drupal.t('Are you sure you want to delete this item?'))) {
                        $.each(events.mousedown, function () {
                            this.handler(); // Invoke the mousedown handlers that was removed.
                        });
                    }

                    // Prevent default action.
                    return false;
                })
            });
        }
    }
})(jQuery);