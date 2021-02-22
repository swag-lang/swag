using swag

#[macro]
func toto(stmt: code)
{
    struct Point
    {
        x, y: s32
    }

    #mixin `stmt
}

//#[swag.printbc]
#test
{
    toto()
    {
        var x: Point
    }
}