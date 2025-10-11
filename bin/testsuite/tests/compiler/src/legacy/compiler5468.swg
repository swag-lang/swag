struct AA
{
    vv   = 2
}

impl AA
{
    #[Swag.Inline]
    func opCmp(me, x: s32)->s32
    {
        return me.vv <=> x
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA
    var a  = 0
    switch x
    {
    case 5 to 10:
        @assert(false)
    default:
        a = 1; break
    }

    @assert(a == 1)
}
