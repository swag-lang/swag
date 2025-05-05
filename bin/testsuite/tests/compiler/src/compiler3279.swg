func toto()
{
    #run
    {
        #assert(#nameof(#self) == "toto")
        #assert(#stringof(#self) == "__compiler3279.toto")
    }
}
