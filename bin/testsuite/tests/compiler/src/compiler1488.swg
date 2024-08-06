#global testerror "Err0459"

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