struct A {}

impl A
{
    mtd toto()->#null *void
    {
        return null
    }

    #[Swag.Optimize("", true)]
    mtd titi()
    {
        if !me.toto() do
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
