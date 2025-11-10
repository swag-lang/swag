using Swag

struct A {}

func get()->A
{
    return {}
}

var G = 0
#[Swag.Overload]
func toto(x: &&A)
{
    G = 1
}

#[Swag.Overload]
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
