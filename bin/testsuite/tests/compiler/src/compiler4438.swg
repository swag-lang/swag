//#global skip
using Swag

func toto(x: string)
{
    @assert(x == "a")
}

//#[Swag.PrintBc]
func toto1(yyy: &string)
{
    toto(yyy)
}

#test
{
    var xxx = "a"
    toto1(&xxx)
}
