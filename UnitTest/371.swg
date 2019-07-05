#module module371;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var slice: [..] s32;	
    slice = {1, 2, 3};	
	@print(cast(s32) slice[0]);
}
