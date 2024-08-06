#global testerror "Err0449"

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