//#global testerror
#test
{
    #[Swag.ConstExpr]
    func isF32(t: typeinfo)->bool
    {
        return t == f32
    }

    struct(var T: isF32(T) = f32) Point
    {
        x, y: T
    }

    var pt: Point
}