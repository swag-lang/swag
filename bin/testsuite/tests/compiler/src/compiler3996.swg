struct V {}

func toto(x: ...)
{
    let k = @kindof(x[0])
    @assert(k.kind == .Pointer)
}

func titi(a: const &V)
{
    toto(a)
}

#test
{
    var x: V
    titi(x)
}
