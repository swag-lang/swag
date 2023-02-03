func print(s: string, f: ...)
    #validifx
    {
        @assert(@countof(f) == 3)
        return true
    }
{
}

#[Swag.Optim("", false)]
#test
{
    print("toto", 1, 2, 3)
}
