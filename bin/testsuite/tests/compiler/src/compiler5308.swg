#global testerror "Err0036"
//#[Swag.PrintBc]
#test
{
    #if true and !@isbytecode()
    {
        @print("toto")
    }
}