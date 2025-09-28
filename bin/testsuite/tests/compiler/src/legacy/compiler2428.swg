func print(s: string, f: ...)
    verify
    {
        @assert(@countof(f) == 3)
        return true
    }
{
}

#[Swag.Optimize("", false)]
#test
{
    print("toto", 1, 2, 3)
}
