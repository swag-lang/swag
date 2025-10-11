using Swag

#[Macro]
func toto(stmt: #code void)
{
    struct Point { x, y: s32 }

    #inject(#up stmt)
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        var x: Point
    }
}
