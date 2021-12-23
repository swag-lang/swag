func toto()
{
    #ast
    {
        #assert @nameof(#self) == "toto"
        #assert #function == "__compiler3278.toto"
        return ""
    }
}
