struct Point2
{
    alias R = s32
    x, y: s32
}

//#[swag.printbc]
func(T) norm(vec: T) -> @typeof(T.R)
{
    #assert(@typeof(T).kind == swag.TypeInfoKind.Struct)
    return vec.x
}

#test
{
    var pt: Point2
    r := norm'Point2(pt)
}