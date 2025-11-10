#global testerror "Err0527"

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
