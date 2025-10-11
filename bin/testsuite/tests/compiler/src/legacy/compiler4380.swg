#[Swag.ConstExpr, Swag.Mixin]
func isF32(t: typeinfo)->bool
{
    //@compilererror("toto", #location(t))
    return t == f32
}

//#global testerror
#test
{
    //#[Swag.ConstExpr, Swag.Mixin]
    /*func isF32(t: typeinfo)->bool
    {
        //@compilererror("toto", #location(t))
        return t == f32
    }*/

    var x = isF32(s32)

    //if x == s32
    {
    }

    //var g: bool = isF32(bool)
    /*func toto(x: bool = isF32(s32))
    {
    }*/

    /*struct(var T: isF32(T) = f32) Point
    {
        x, y: T
    }

    var pt: Point's32*/
}
