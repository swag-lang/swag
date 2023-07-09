func(T) toto(x: T)
{
    @assert(false)
}

func(T) toto(x: const [..] T)
{
}

#test
{
    var x = 0
    toto(@mkslice(cast(^s32) &x, 1))
}