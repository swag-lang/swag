#global testerror

#[Swag.Macro]
func toto(x: string)
{
    #run
    {
        @compilererror("error", @location(x))
    }
}

#test
{
    toto("val")
}