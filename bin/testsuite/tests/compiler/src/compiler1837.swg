
#global testerror "Err0259"

struct Point2
{
    typealias R = s32
    x, y: s32
}

struct Point3
{
    typealias R = s32
    x, y, z: s32
}

//#[Swag.PrintBc]
func(T) norm(vec: T) -> @typeof(T.R)
{
    return vec.x
}

#test
{
    var pt2: Point2
    norm'Point2(pt2)

    var pt3: s32
    norm's32(pt3)
}