using Swag

struct A {}

func get() -> A
{
    return {}
}

var G = 0
func toto(x: moveref A)
{
    G = 1
}

func toto(x: A)
{
    @assert(false)
}

//#[Swag.PrintBc]
#test
{
    toto(get())
	@assert(G == 1)
}
