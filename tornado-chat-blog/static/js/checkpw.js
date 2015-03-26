function confirmed(){
	var passWord = document.getElementsByName('password')[0].value;
	var passWordDemo = document.getElementsByName('confirm-password')[0].value;
	var userName = document.getElementsByName("username")[0].value;
	if (userName=='')
	    return;
	if (passWord=='')
	    alert("Please enter your password")
	if(passWordDemo　!= passWord)
	    alert("please confirm your password!")
} 
