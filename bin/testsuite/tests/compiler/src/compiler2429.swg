#global testerror

func print(s: string, f: ...)
    #selectifx
    {
        if @countof(f) != 3
            @compilererror("invalid number of arguments", @location(f))
        return true
    }
{
}

#[Swag.Optim("", false)]
#test
{
    print("toto", 1, 2)
}
