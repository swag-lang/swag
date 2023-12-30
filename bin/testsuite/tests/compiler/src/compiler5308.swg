#global testerror "Err0798"
//#[Swag.PrintBc]
#test
{
    #if true and !@isbytecode()
    {
        @print("toto")
    }
}