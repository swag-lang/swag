struct A
{
    xx:  s32
}

//#[Swag.PrintBc]
impl A
{
    #[Swag.Macro]
    func mac(me)
    {
        me.xx = 666
    }

    func toto(me)
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
