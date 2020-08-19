#module "compiler1419"
#unittest error

struct A
{
    x, w: s64
}

impl A
{
    #[swag.property]
    func right(s, t: s32)->s32
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