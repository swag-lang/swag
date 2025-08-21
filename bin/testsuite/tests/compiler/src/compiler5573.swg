#global testerror "Err0487"

struct A
{
    x, w: s64
}

impl A
{
    //#[Swag.property]
    func right(using me, a, b, c, d: s32)->s64
    {
        return x + w
    }
}

#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    @assert(pt.right() == 11)
    //pt.right = 10
}