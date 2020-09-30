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
	@assert(F1 == 0)
	@assert(F2 == 1)
	@assert(F3 == 2)
	@assert(F4 == 4)
}
