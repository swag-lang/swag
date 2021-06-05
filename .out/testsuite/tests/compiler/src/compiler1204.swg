struct A
{
    x: u32
}

impl A
{
    #[Swag.inline]
    func toto(v: u32) => v
}

//#[Swag.printbc]
#test
{
    var pt: A
    @assert(A.toto(666) == 666)
}