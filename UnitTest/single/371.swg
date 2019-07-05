#module module371;
//#unittest pass semantic;

//#[swag.printbytecode]
#test
{
	var slice: [..] s32;	
    slice = {1, 2, 3};	
	@assert(slice[0] == 1);
	@assert(slice[1] == 2);
	@assert(slice[2] == 3);
	slice = {10, 20, 30};	
	@assert(slice[2] == 30);
}
