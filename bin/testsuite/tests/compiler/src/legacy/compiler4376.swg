#global testerror "Err0403"
#test
{
    #[Swag.ConstExpr]
    func isFloat(t: typeinfo) => t == f32 or t == f64

    struct(var T) Point
            where isFloat(T)
    {
        x, y: T
    }

    var pt: Point's32
}
