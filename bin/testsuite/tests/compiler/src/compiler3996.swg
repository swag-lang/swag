struct V {}

func toto(x: ...)
{
    var k = @kindof(x[0])
    @assert(k.kind == .Pointer)
}

func titi(a: const ref V)
{
    toto(a)
}

#test
{
    var x: V
    titi(x)
}