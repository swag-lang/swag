struct AA
{

}

impl AA
{
    #[Swag.implicit]
    func opCast(using self)->s32 => 666
}

func toto(v: s32)
{
    @assert(v == 666)
}

#test
{
    var v: AA
    var ptr = &v
    ptr.toto()
}