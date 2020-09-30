#[swag.enumflags]
enum TOTO : u8
{
	F1 = 3
	F2 = 0b100
	F3
	F4
}

#test
{
	@assert(TOTO.F3 == 0b1000)
}
