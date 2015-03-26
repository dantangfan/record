var count=0;
$(document).ready(function(){
  $(".help").hide();
  $("#help").click(function(){
    if (count%2==0){
      $(".help").show(1000);
    }
    else{
      $(".help").hide(1000);
    }
    count = count+1;
  });
});
