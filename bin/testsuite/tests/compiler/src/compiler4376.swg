#global testerror "Err0088"
#test
{
    #[Swag.ConstExpr]
    func isFloat(t: typeinfo) => t == f32 or t == f64

    struct(var T: isFloat(T)) Point
    {
        x, y: T
    }

    var pt: Point's32
}
