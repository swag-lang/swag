#module module287;
//#unittest pass backend;

//#[swag.printbytecode]
#[swag.test]
func test1: ()
{
	var a: bool = true;
	var b = &a;
	var c = b[0];
	@assert(c == true);
}

#[swag.test]
func test2: ()
{
	var a: s32 = 666;
	var b = &a;
	var c = b[0];
	@assert(c == 666);
}