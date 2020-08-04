struct Point2
{
    alias R = s32
    x, y: s32
}

//#[swag.printbc]
func(T) norm(vec: T) -> @typeof(T.R)
{
    return vec.x
}

#test
{
    var pt: Point2
    norm'Point2(pt)
}