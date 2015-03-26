$(function(){
    $(".new-topic form").hide();
    $(".new-topic a").click(function(){
        $(".new-topic form").toggle(1000);
    });
    
    $(".click").click(function(){
        $('.bg').fadeIn(300);
        $('.login').fadeIn(500);
    });
    
    $(".bg").click(function(){
        $(".bg").fadeOut(300);
        $(".login").fadeOut(300);
    });
    
    $(".btn").click(function(){
        var pass = $("#password").val();
        var idvalue = $(".p3").attr("title");
        window.location.pathname = '/chating/'+idvalue+'?password='+pass;
    });
});


