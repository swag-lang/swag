struct X
{
}

impl X
{
    #[Swag.Macro]
    mtd toto()
    {
        loop idx: 3
        {
            #macro(idx)
            {
            }
        }
    }
}

#test
{
    var x: X
    x.toto()
}