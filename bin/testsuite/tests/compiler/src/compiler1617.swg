struct A
{
    xx: s32
}

//#[Swag.PrintBc]
impl A
{
    #[Swag.Macro]
    func mac(using me)
    {
        xx = 666
    }

    func toto(using me)
    {
        mac(me)
    }
}

#test
{
    var pt: A
    pt.toto()
    @assert(pt.xx == 666)
}