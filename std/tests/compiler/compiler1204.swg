struct A
{
    x: u32
}

impl A
{
    #[swag.inline]
    func toto(v: u32) => v
}

//#[swag.printbc]
#test
{
    var pt: A
    @assert(A.toto(666) == 666)
}