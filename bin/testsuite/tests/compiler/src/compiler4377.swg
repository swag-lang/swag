#global testerror "Err0088"
#test
{
    #[Swag.ConstExpr]
    func isF32(t: typeinfo) => t == f32
    #[Swag.ConstExpr]
    func isF64(t: typeinfo) => t == f64

    struct(var T: isF32(T) or isF64(T)) Point
    {
        x, y: T
    }

    var pt: Point's32
}
