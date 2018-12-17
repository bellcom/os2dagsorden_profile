(function($){
    $.fn.focusTextToEnd = function(){
        this.focus();
        var $thisVal = this.val();
        this.val('').val($thisVal);
        return this;
    }
}(jQuery));

jQuery(document).ready(function($) {
    $("#edit-body-und-0-value").focusTextToEnd();  
});
