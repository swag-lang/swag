#module compiler394
//#unittest pass semantic;

#unittest error;
var c = 666;
var d = {c};
	
//#[swag.printbc]
#test
{
	//@print(d[0]);
	//@print("这是什么");
}
