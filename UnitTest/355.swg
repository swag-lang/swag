#module module355;
//#unittest pass semantic;

var a = {"0", "1.0"};

#[swag.printbytecode]
#test
{
	@print(cast(s32) @prop(a[0], count));
}
