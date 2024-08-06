#global testerror "Err0044"
//#[Swag.PrintBc]
#test
{
    #if true and !@isbytecode()
    {
        @print("toto")
    }
}