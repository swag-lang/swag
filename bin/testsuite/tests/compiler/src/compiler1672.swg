//
//#global testerror

//#[Swag.PrintBc]
func toto(x: ...)
{
    @assert(@typeof(x[0]).fullname == "any")
}

#run
{
    toto(5)
}
