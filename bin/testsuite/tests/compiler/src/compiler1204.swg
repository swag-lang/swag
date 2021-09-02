struct A
{
    x: u32
}

impl A
{
    #[Swag.Inline]
    func toto(v: u32) => v
}

//#[Swag.PrintBc]
#test
{
    var pt: A
    @assert(A.toto(666) == 666)
}