#global testerror "Err0256"

//#[Swag.Inline]
func toto(x: #code void) {}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        @print("toto\n")
    }
}
