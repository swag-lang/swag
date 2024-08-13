#global testerror "Err0401"

//#[Swag.Inline]
func toto(x: code)
{
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        @print("toto\n")
    }
}