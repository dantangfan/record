$(function(){
    $("#divMsg").ajaxStart(function(){
        $(this).show();
    });
    
    $("#divMsg").ajaxStop(function(){
        $(this).html("已完成").hide();
    });
    
    InitFace();
    GetMessageList();
    
    $("#Button1").click(function(){
        var $content = $("#txtContent");
        if($content.val() != ""){
            SendContent($content.val());
        }
        else{
            alert("发送不能为空");
            $content.focus();
            return false;
        }
    });
    
    $("table tr td img").click(function(){
        var strContent = $("#txtContent").val()+"<:"+this.id+":>";
        $("#textContent").val(strContent);
    });
    
    $(".del").click(function(){
        $.ajax({
            type:"POST",
            url:"/delChat",
            data:{'idvalue':window.location.pathname.split("/")[2]},
            success:function(data){
                if(data=='true'){
                    window.location.pathname='/chat';
                }
                else{
                    alert("操作失败，请稍后重试。");
                }
            }
        });
    });
});


function SendContent(content){
    $.ajax({
        type:"POST",
        url:window.location.pathname,
        data:{
            msg:content
        },
        success:function(data){
            if(data){
                GetMessageList();
                $("#txtContent").val("");
            }
            else{
                alert("发送失败");
                return false;
            }
        }
    });
}


function GetMessageList(){
    $.ajax({
        type:"POST",
        url:window.location.pathname,
        data:{msg:""},
        success:function(data){
            var mstr="";
            var pstr="";
            for(var i = 0; i<data["msgList"].length;i++){
                M = data["msgList"][i];
                name = M[0];
                time = M[1];
                msg = M[2];
                mstr += "<div class='msg'>"+"<div class='up'>"+"<p class='name'>"+name+"</p>"+"<p class='posttime'>"+time+"</p>"+"<div style='clear:both;'></div>"+"</div>"+"<div class='down'>"+msg+"</div>"+"</div>"
            }
            for(var j=0; j<data["peopleList"].length; j++){
                p = data["peopleList"][j];
                pstr += "<div class='ponline'>"+p+"</div>";
            }
            $("#divContent").html(mstr);
            $("#divOnLine").html(pstr);
        }
    });
    AutoUpdContent();
}

function InitFace(){
    var strHTML="";
    for (var i = 1; i<= 10; i++){
        strHTML += "<img src='../static/images/Face/"+i+".gif' id='"+i+"'/>";
    }
    $("#divFace").html(strHTML);
}


function AutoUpdContent(){
    setTimeout(GetMessageList,4000);
}
