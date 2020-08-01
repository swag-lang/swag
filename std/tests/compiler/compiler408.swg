

//#[swag.printbc]
#test
{
	var c: const [..] u32 = @[0x01_02_03_04];
	var d = cast(const [..] u8) c;
	@assert(@countof(d) == 4);
	@assert(d[0] == 0x04);
	@assert(d[1] == 0x03);
	@assert(d[2] == 0x02);
	@assert(d[3] == 0x01);
}