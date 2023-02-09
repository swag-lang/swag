//#global skip
using Swag

func toto(x: string)
{
	@assert(x == "a")
}

//#[Swag.PrintBc]
func toto1(yyy: ref string)
{
	toto(yyy)
}

#test
{
	xxx := "a"
	toto1(&xxx)
}
