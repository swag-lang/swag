#global testerror "Err0399"

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