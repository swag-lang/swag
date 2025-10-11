//
struct A {}
struct B {}

impl A
{
    mtd toto()
    {
        var b: B
        b.titi()
    }
}

impl B
{
    mtd tata() {}

    #[Swag.Mixin]
    func titi(me)
    {
        me.tata()
    }
}

#test
{
    var a: A
    a.toto()
}
