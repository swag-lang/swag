struct A {}

impl A
{
    mtd tata()
    {
        return
    }

    #[Swag.Mixin]
    mtd titi()
    {
        .tata()
    }
}

#test
{
    var a: A
    a.titi()
}
