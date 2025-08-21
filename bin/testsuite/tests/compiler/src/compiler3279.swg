func toto()
{
    #run
    {
        #assert(#nameof(#me) == "toto")
        #assert(#stringof(#me) == "__compiler3279.toto")
    }
}
