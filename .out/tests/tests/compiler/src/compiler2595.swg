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

//#[Swag.printbc]
#test
{
    toto()
    {
        var x: Point
    }
}