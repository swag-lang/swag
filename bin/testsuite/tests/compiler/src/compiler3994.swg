struct A {}

var G: *void

func toto(x: any)
{
    let k = @kindof(x)
    @assert(G != @dataof(x))
    @assert(G == dref cast(**void) @dataof(x))
    @assert(k.kind == .Pointer)
}

//#[Swag.PrintBc]
func titi(a: const &A)
{
    toto(a)
}

#test
{
    var a: A
    G = &a
    titi(a)
}
