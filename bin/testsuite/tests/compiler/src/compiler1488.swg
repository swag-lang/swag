#global testerror "Err0410"

//#[Swag.Inline]
func toto(x: #block void)
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