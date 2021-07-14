#[Swag.EnumFlags]
enum TOTO : u32
{
	F1 = 0x00000001
	F2 = 0x00000002
	F3 = F1 | F2
}

#test
{
	@assert(TOTO.F3 == 3)
}
