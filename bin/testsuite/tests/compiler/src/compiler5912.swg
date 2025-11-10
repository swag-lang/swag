struct A {}

impl A
{
    mtd tata()
    {
        return
    }

    mtd toto()
    {
        #[Swag.Mixin]
        func titi()
        {
            .tata()
        }
    }
}
