struct A
{
    xx: s32
}

//#[Swag.PrintBc]
impl A
{
    #[Swag.Macro]
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