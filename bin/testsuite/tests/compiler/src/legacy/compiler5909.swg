struct A
{
    x    = 0
}

impl A
{
    #[Swag.Macro]
    mtd titi()
    {
        if !.x:
            return
    }
}

#test
{
    var a: A
    a.titi()
}
