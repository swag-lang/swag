#global testerror "invalid number of arguments"

func print(s: string, f: ...)
    check
    {
        if @countof(f) != 3 do
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
