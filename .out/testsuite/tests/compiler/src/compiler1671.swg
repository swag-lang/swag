//
//#global testerror

//#[Swag.printbc]
func toto(x: ...)
{
    @assert(@kindof(x[0]).name == "s32")
}

#run
{
    toto(5)
}