#global testerror "Err0410"

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