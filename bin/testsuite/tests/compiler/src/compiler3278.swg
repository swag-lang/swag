func toto()
{
    #ast
    {
        #assert(#nameof(#me) == "toto")
        #assert(#stringof(#me) == "__compiler3278.toto")
        return ""
    }
}
