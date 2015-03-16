/*function onload(){
  var d=new Date();
  var day=d.getDate();
  var month=d.getMonth() + 1;
  var year=d.getFullYear();
  today = year + "-" + month + "-" + day;
  tasks = document.getElementsByName("task");
  for(i=0;i<tasks.length;i++){
    start = tasks[i].getElementsByClassName("start")[0].innerText;
    end = tasks[i].getElementsByClassName("end")[0].innerText;
    sum = DateDiff(end,start);
    sub = DateDiff(today,start);
    img = tasks[i].getElementsByClassName("colored")[0];
    //name = tasks[i].getElementsByClassName("key")[0].innerText;
    //container = document.getElementById("bar"+name);
    percent = sub/sum*100;
    img.style.width=percent+'%';
    //var colored = document.createElement('div');
    //colored.className = 'colored';
    //colored.style.width = percent+'%';
    //container.appendChild(colored);
  }
}
*/

$(function(){
  var d=new Date();
  var day=d.getDate();
  var month=d.getMonth() + 1;
  var year=d.getFullYear();
  today = year + "-" + month + "-" + day;
  tasks = document.getElementsByName("task");
  for(i=0;i<tasks.length;i++){
    start = tasks[i].getElementsByClassName("start")[0].innerText;
    end = tasks[i].getElementsByClassName("end")[0].innerText;
    sum = DateDiff(end,start);
    sub = DateDiff(today,start);
    img = tasks[i].getElementsByClassName("colored")[0];
    //name = tasks[i].getElementsByClassName("key")[0].innerText;
    //container = document.getElementById("bar"+name);
    percent = sub/sum*100;
    percent = Math.round(percent);
    img.style.width=percent+'%';
    if (percent>100)
      percent = 100;
    if (percent<0)
      percent = 0;
    if (percent>50){
      img.src="/static/images/color1.png";
    }
    //var colored = document.createElement('div');
    //colored.className = 'colored';
    //colored.style.width = percent+'%';
    //container.appendChild(colored);
  }
});


/*d1--end,d2---start*/
function DateDiff(d1,d2){
  var day = 24 * 60 * 60 *1000;
  try{    
    var dateArr = d1.split("-");
    var checkDate = new Date();
    checkDate.setFullYear(dateArr[0], dateArr[1]-1, dateArr[2]);
    var checkTime = checkDate.getTime();
    var dateArr2 = d2.split("-");
    var checkDate2 = new Date();
    checkDate2.setFullYear(dateArr2[0], dateArr2[1]-1, dateArr2[2]);
    var checkTime2 = checkDate2.getTime();
    var cha = (checkTime - checkTime2)/day;  
    return cha;
    }catch(e){
      return false;
  }
}

function getToday(){
  var d=new Date();
  var day=d.getDate();
  var month=d.getMonth() + 1;
  var year=d.getFullYear();
  today = year + "-" + month + "-" + day;
  return today;
}


function showCompleted(dom){
  
  percent = dom.getElementsByTagName('img')[0].style.width;
  temp = parseInt(percent);
  if(temp<=25)
    word = "时间还很充裕，刚过"+percent+",不激动，慢慢来亲！";
  else if(temp<=50)
    word = "已经过去"+percent+"了，项目进行多少了呀？";
  else if(temp<=60)
    word = "时间过的真快呀！转眼就"+percent+"了！";
  else if(temp<=70)
    word = "恩，时间过了"+percent+"了，再叫改方案你就跪了。";
  else if(temp<=85)
    word = "Oh.. shit!"+percent+"学委在催了";
  else if(temp<=95)
    word = "过了"+percent+"了，差不多完成了吧";
  else
    word = percent+"，没啥时间了，再挣扎一下吧。。。";
  dom.title = word;//"时间已经过了"+percent+"，继续加油亲！";
  //alert(percent+ 'complete');
}

function showbox(){
  item = document.getElementById('newtask').getElementsByTagName('div');
  /*//默认起始时间是今天
  var d=new Date();
  var day=d.getDate();
  var month=d.getMonth() + 1;
  var year=d.getFullYear();
  today = year + "-" + month + "-" + day;
  item[0].getElementsByClassName("time")[0].value=today;*/
  item[0].className="showbox";
  document.getElementById('add').className="hide";
}

function failed(){
  document.getElementById('newtask').getElementsByTagName('div')[0].className="hide";
  document.getElementById('newtask').getElementsByTagName('button')[0].className="add";
}

function checkEndDate(){
  endDate = document.getElementById("form").getElementsByTagName("input")[0].value;
  today = getToday();
  charge = DateDiff(endDate,today);
  if (charge<0)
    alert("你个逗逼，截至日期不能小于今天啦！");
}
function succeed(){
  taskname = document.getElementsByTagName('textarea').value;
  startdate=new Date()
  enddate=document.getElementsByName('time')[0].value;
  var node = document.createElement('div');
}

