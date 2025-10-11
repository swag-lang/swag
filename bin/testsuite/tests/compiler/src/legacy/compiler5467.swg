struct AA
{
    vv   = 6
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
        a = 1; break
    default:
        @assert(false)
    }

    @assert(a == 1)
}
