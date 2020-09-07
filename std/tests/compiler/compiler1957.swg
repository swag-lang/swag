struct A
{
    x: s32
}

impl A
{
    func opPostCopy(self)
    {
        @assert(false)
    }
}

//#[swag.printbc]
#test
{
    var x, y: A
}