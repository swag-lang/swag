func toto()
{
    #assert(#curlocation.funcName == "toto")

    var fct = func()
    {
        #assert(#curlocation.funcName != "")
    }
}
