#[Swag.ConstExpr, Swag.Mixin]
func isF32(t: typeinfo)->bool
{
    //@compilererror("toto", #location(t))
    return t == f32
}

//#global testerror
#test
{
    //var g: bool = isF32(bool)
    func toto(x: bool = isF32(f32))
    {
        @assert(x == true)
    }

    toto()
    /*struct(var T: isF32(T) = f32) Point
    {
        x, y: T
    }

    var pt: Point's32*/
}