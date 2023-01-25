#global testerror

#[Swag.Macro]
func toto(x: string)
{
    #run
    {
        @error("error", @location(x))
    }
}

#test
{
    toto("val")
}