var G = 0

func(T) toto(x: *void)
{
    G += 2
}

func(T) toto(x: *T)
{
    G += 1
}

#test
{
    var a: s32
    toto(&a)
    toto(&a)
    @assert(G == 2)
}