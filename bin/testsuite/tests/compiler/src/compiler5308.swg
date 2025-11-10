#global testerror "Err0548"
//#[Swag.PrintBc]
#test
{
    #if true and !@bytecode
    {
        @print("toto")
    }
}
