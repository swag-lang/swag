#global testerror

func print(s: string, f: ...)
    #checkif
    {
        if @countof(f) != 3 @errormsg("invalid number of arguments", #location(f))
        return true
    }
{
}

#[Swag.optim("", false)]
#test
{
    print("toto", 1, 2)
}
