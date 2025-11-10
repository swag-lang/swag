//#global testerror
#test
{
    #[Swag.ConstExpr]
    func isF32(t: typeinfo) => t == f32

    struct(var T = f32) Point
            where isF32(T)
    {
        x, y: T
    }

    var pt: Point
}
