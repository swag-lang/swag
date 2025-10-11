func(T) toto(x: T)
{
    #assert(#typeof(x) == s32)
}

#test
{
    toto(x: 1's32)
}

func(T, V) titi(x: T, y: V)
{
    #assert(#typeof(x) == s32)
    #assert(#typeof(y) == f32)
}

#test
{
    titi(y: 1'f32, x: 1's32)
}
