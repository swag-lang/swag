;
;

//#[Swag.PrintBc]
#test
{
	var arr: [] s32 = @[1,2];
	@assert(@countof(arr) == 2);
	@assert(@sizeof(arr) == 8);
}
