func print(s, s1: string, f: ...)
    verify
    {
        //@print(cast(s64) @countof(f))
        @assert(s == "toto")
        @assert(s1 == "titi")
        return true
    }
{
}

#[Swag.Optimize(.All, false)]
#test
{
    //print("toto", 1, 2, 3)
    print("toto", "titi")
}
