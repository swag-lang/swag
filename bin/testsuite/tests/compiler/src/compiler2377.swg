func toto(x: const [..] u8...)
{
    @assert(cast(string) x[0] == "a")
    @assert(cast(string) x[1] == "b")
}

#test
{
    var x = ["a", "b"]
    toto(@spread(x))
}
