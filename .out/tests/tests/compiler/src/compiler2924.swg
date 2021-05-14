func(T) toto(x: T)
{
    @assert(false)
}

func(T) toto(x: const [..] T)
{
}

#test
{
    x := 0
    toto(@mkslice(&x, 1))
}