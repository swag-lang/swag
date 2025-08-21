func toto()
{
    #assert(#nameof(#me) == "toto")
    #assert(#stringof(#me) == "__compiler3277.toto")

    var fct = func()
    {
        #assert(#nameof(#me) != "")
    }
}
