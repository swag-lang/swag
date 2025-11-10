struct AA {}

impl AA
{
    #[Swag.Implicit]
    func opCast(me)->s32 => 666
}

func toto(v: s32)
{
    @assert(v == 666)
}

#test
{
    var v:  AA
    let ptr = &v
    ptr.toto()
}
