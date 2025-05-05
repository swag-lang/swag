func toto()
{
    #ast
    {
        #assert(#nameof(#self) == "toto")
        #assert(#stringof(#self) == "__compiler3278.toto")
        return ""
    }
}
