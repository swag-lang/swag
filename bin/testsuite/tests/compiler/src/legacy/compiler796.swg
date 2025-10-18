#global testerror "Err0581"

//#[Swag.PrintBc]
func(x: u8) toto()
{
    @print(x)
}

//#[Swag.PrintBc]
#test
{
    toto'u8()
}
