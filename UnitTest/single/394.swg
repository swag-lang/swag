#module module394;
//#unittest pass semantic;

#[swag.printbytecode]
#test
{
	c := 666;
	d := {c};
	@print(d[0]);
	//@print("这是什么");
}
