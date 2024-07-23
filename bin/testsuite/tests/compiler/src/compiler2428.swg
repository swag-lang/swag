func print(s: string, f: ...)
    where(each)
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
