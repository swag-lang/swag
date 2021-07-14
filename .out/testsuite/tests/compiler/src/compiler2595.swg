using Swag

#[macro]
func toto(stmt: code)
{
    struct Point
    {
        x, y: s32
    }

    #mixin `stmt
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        var x: Point
    }
}