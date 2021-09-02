func toto(x: const [..] u8...)
{
    @assert(cast(string) x[0] == "a")
    @assert(cast(string) x[1] == "b")
}

#test
{
    x := @["a", "b"]
    toto(@spread(x))
}