//#global skip
using Swag

func toto(x, y: ref string)
{
	x =,move y
	@assert(x == "1")
}

#test
{
	x := "0"
	y := "1"
	toto(&x, &y)
	@assert(x == "1")
}
