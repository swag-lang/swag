#global testerror

var G = 0

//#[Swag.PrintBc]
func toto(str: string)
    #selectifx
    {
        if !@isconstexpr(str)
            return true
        @compilererror("toto", @location(str))
        return true
    }
{
    G += 10
}

#test
{
    toto("a")
}