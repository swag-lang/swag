func print(s: string, f: ...)
    #checkif
    {
        @assert(@countof(f) == 3)
        return true
    }
{
}

#[Swag.optim("", false)]
#test
{
    print("toto", 1, 2, 3)
}
