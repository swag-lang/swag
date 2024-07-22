#[Swag.ConstExpr, Swag.Mixin]
func isF32(t: typeinfo)->bool
{
    //@compilererror("toto", @location(t))
    return t == f32
}

//#global testerror
#test
{
    struct(var T where isF32(T)) Point
    {
        x, y: T
    }

    var pt: Point'f32
}