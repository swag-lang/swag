func toto()
{
    #run
    {
        #assert(#curlocation.funcName == "toto")
        //#assert(#stringof(#me) == "__compiler3279.toto")
    }
}
