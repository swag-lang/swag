struct Point2
{
    alias R = s32
    x, y: s32
}

//#[Swag.PrintBc]
func(T) norm(vec: T)->#decltype(T.R)
{
    #assert(#typeof(T).kind == Swag.TypeInfoKind.Struct)
    return vec.x
}

#test
{
    var pt: Point2
    var r   = norm'Point2(pt)
}
