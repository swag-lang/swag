#global testerror

var G = 0

//#[Swag.PrintBc]
func toto(str: string)
    #selectif
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