#global testerror "invalid number of arguments"

func print(s: string, f: ...)
    verify
    {
        if @countof(f) != 3:
            @compilererror("invalid number of arguments", #location(f))
        return true
    }
{
}

#[Swag.Optimize("", false)]
#test
{
    print("toto", 1, 2)
}
