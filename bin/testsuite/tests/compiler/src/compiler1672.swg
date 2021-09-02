//
//#global testerror

//#[Swag.PrintBc]
func toto(x: ...)
{
    @assert(@typeof(x[0]).name == "any")
}

#run
{
    toto(5)
}