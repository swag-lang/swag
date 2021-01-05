
#global testerror

struct A
{
    x, w: s64
}

impl A
{
    #[swag.property]
    func right(s, y, z: s32)
    {
    }
}

#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    //A.right(pt) = 10
}