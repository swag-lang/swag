#global testerror "Err0421"
//#[Swag.PrintBc]
#test
{
    #if true and !@bytecode
    {
        @print("toto")
    }
}
