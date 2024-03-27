#global testerror "error"

#[Swag.Macro]
func toto(x: string)
{
    #run
    {
        var loc = @location(x)
        loc.colStart += 1
        loc.colEnd -= 1
        @compilererror("error", loc)
    }
}

#test
{
    toto("val")
}
