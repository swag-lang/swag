#global testerror "Err0798"
#[Swag.PrintBc]
#test
{
    #if #cfg == "fast-debug" and !@isbytecode()
    {
        @print("toto")
    }
}