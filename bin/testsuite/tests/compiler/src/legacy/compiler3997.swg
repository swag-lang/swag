struct A { a, b, c, d: s32 }

var G: *void

func toto(x: any)
{
    let k = @kindof(x)
    @assert(G != @dataof(x))
    @assert(G == dref cast(**void) @dataof(x))
    @assert(k.kind == .Pointer)
}

func tata(x: ...)
{
    toto(x[0])
}

//#[Swag.PrintBc]
func titi(a: const &A)
{
    tata(a)
}

#test
{
    var a: A
    G = &a
    titi(a)
}
