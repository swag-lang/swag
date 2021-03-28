/*
#global testerror

struct A
{
    x, w: s64
}

impl A
{
    #[swag.property]
    func right()
    {
    }
}

#test
{
    var pt: A
    pt.x = 1
    pt.w = 10
    //A.right(pt) = 10
}*/