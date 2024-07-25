#global testerror "toto"

var G = 0

//#[Swag.PrintBc]
func toto(str: string)
    where:call
    {
        if !@isconstexpr(str) do
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