struct A
{
    xx: s32
}

//#[swag.printbc]
impl A
{
    #[swag.macro]
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