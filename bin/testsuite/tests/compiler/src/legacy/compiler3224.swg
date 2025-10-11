struct A {}

impl A
{
    mtd toto() {}
}

struct C
{
    using a: A
}

impl C
{
    mtd titi()
    {
        me.toto()
    }
}
