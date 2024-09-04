#global testerror "Err0039"
//#[Swag.PrintBc]
#test
{
    #if true and !@isbytecode()
    {
        @print("toto")
    }
}