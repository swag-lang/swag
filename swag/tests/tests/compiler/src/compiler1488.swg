
#global testerror

//#[swag.inline]
func toto(x: code)
{
}

//#[swag.printbc]
#test
{
    toto()
    {
        @print("toto\n")
    }
}