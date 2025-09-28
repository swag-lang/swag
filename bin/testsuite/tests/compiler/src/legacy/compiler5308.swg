#global testerror "Err0040"
//#[Swag.PrintBc]
#test
{
    #if true and !@bytecode
    {
        @print("toto")
    }
}