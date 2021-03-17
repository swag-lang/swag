#[swag.enumflags]
enum TOTO
{
	F1
	F2
	F3
	F4
}

#test
{
	using TOTO
	@assert(F1 == 1)
	@assert(F2 == 2)
	@assert(F3 == 4)
	@assert(F4 == 8)
}
