#global testerror

func print(s: string, f: ...)
    #selectif
    {
        if @countof(f) != 3 @error("invalid number of arguments", #location(f))
        return true
    }
{
}

#[swag.optim("", false)]
#test
{
    print("toto", 1, 2)
}
