#global testerror "Err0024"
//#[Swag.PrintBc]
#test
{
    #if true and !@isbytecode()
    {
        @print("toto")
    }
}