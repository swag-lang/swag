#module module408

#[swag.printbytecode]
#test
{
	var c: [..] u32 = {0x01_02_03_04};
	var d = cast!([..] u8)(c);
	@assert(@prop(d, count) == 4);
	@assert(d[0] == 0x04);
	@assert(d[1] == 0x03);
	@assert(d[2] == 0x02);
	@assert(d[3] == 0x01);
}