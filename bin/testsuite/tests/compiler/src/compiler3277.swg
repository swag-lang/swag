func toto()
{
    #assert @nameof(#self) == "toto"
    #assert @stringof(#self) == "__compiler3277.toto"

    fct := @()
    {
        #assert @nameof(#self) != ""
    }
}
