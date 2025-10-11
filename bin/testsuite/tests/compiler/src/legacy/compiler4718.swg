alias A = u8

func(T) toto()
{
    @assert(#typeof(T).name == "u8")
}

#test
{
    toto'A()
    toto'u8()
}
