struct AA
{
    ptr: *AA
}

impl AA
{
    mtd titi() {}

    mtd toto()
    {
        with ptr
        {
            me.titi()
        }
    }
}

#test
{
    var v: AA
    v.toto()
}
