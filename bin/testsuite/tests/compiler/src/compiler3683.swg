struct A {}

impl A
{
    mtd toto()->*void
    {
        return null
    }

    #[Swag.Optim("", true)]
    mtd titi()
    {
        if !toto() do
            return
        @assert(false)
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    x.titi()
}