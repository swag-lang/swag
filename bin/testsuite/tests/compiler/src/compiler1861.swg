func toto(ptr: const ^u8)
{
    let x = @mkstring(ptr, 4)
    //@print(@typeof(x).name)
    //@print("x")
    @assert(x == "toto")
}

#test
{
    toto(cast(const ^u8) "toto")
}
