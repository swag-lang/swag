#[Swag.ConstExpr, Swag.Mixin]
func isF32(t: typeinfo)->bool
{
    //@compilererror("toto", @location(t))
    return t == f32
}

//#global testerror
#test
{
    //#[Swag.ConstExpr, Swag.Mixin]
    /*func isF32(t: typeinfo)->bool
    {
        //@compilererror("toto", @location(t))
        return t == f32
    }*/

    var x = isF32(s32)

    //if x == s32
    {
    }
}
