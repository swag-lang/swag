func print(s: string, f: ...)
    where<call>
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
