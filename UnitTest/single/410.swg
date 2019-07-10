#module module410

#[swag.printbytecode]
#test
{
	var c = {0};
	var d: [..] *s32 = {&c[0]};
	//d[0,0] = 1;
}