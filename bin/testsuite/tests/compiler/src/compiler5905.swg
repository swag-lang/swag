struct B {}

impl B
{
    mtd titi() throw
    {
    }

    mtd toto()
    {
        assume .titi()
    }
}
