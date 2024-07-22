#global testerror "Err0084"
#test
{
    #[Swag.ConstExpr]
    func isF32(t: typeinfo) => t == f32
    #[Swag.ConstExpr]
    func isF64(t: typeinfo) => t == f64

    struct(var T) Point #validif isF32(T) or isF64(T)
    {
        x, y: T
    }

    var pt: Point's32
}