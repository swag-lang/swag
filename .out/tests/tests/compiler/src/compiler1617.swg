struct A
{
    xx: s32
}

//#[Swag.printbc]
impl A
{
    #[Swag.macro]
    func mac(using self)
    {
        xx = 666
    }

    func toto(using self)
    {
        mac(self)
    }
}

#test
{
    var pt: A
    pt.toto()
    @assert(pt.xx == 666)
}