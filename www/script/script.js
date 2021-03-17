$(function() {
    $('.menu-button').click(function() {
        if ($('.category-list').css('visibility') == 'hidden') {
            $('.menu-button').css({
                transform: 'rotate(90deg) translateZ(0)',
                transition: '0.4s'
            });
            $('.menu-button_type_line-2').css({
                'transform': 'translateX(-100%)',
                opacity: '0',
                transition: '.3s .1s'
            });
            $('.category-list').css({
                visibility: 'visible',
                transform: 'none',
                transition: '0.4s'
            });
        } else {
            $('.menu-button').css('transform', 'none'), $('.menu-button_type_line-2').css({
                'transform': 'none',
                opacity: '1'
            }), $('.category-list').css({
                visibility: 'hidden',
                transform: 'translateY(-20px)'
            });
        }
    });
    $(window).resize(function() {
        if ($(window).width() > 1024) {
            $('.category-list').removeAttr('style');
        }
    });
    $('.request-button, .request-ft-button').on('click', function() {
        $('.popup-container').css('display', 'flex');
        $("body").addClass("fixed");
    });
    $('.popup__button').click(function() {
        if ($('#confid').is(':checked')) {
            $('.label-check').css('color', 'grey');
        } else {
            $('.label-check').css('color', 'red');
        }
    });
    $('.label-check').on('click', function() {
        $('.label-check').css('color', 'grey');
    });
    $('.popup-container').on('click', function(event) {
        if (event.target === this) {
            $('.popup-container').css('display', 'none');
            $("body").removeClass("fixed");
        }
    });
    $('.popup__button').on('click', function() {
        var ggg = $('.form__item').val();
        if (ggg == 0) {
            $('.label-check').css('color', 'red');
        } else {
            var data = {
                'name': $('.form input[name="name"]').val(),
                'phone': $('.form input[name="phone"]').val(),
                'email': $('.form input[name="email"]').val()
            };
            $.post('/recall.php', data, function(response) {
                if (response == 'OK') {
                    $('.popup-container').css('display', 'none');
                }
            });
        }
    });
});