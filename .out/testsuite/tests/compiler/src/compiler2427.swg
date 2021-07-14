func print(s, s1: string, f: ...)
    #checkif
    {
        //@print(cast(s64) @countof(f))
        @assert(s == "toto")
        @assert(s1 == "titi")
        return true
    }
{
}

#[Swag.Optim("", false)]
#test
{
    //print("toto", 1, 2, 3)
    print("toto", "titi")
}
