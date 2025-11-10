struct AA
{
    vv   = 6
}

impl AA
{
    func opCmp(me, x: s32)->s32
    {
        return me.vv <=> x
    }
}

#test
{
    var x: AA
    switch x
    {
    case 5 to 10:
        break
    default:
        @assert(false)
    }

    switch x
    {
    case 0 to 5:
        @assert(false)
    case 6 to 10:
        break
    default:
        @assert(false)
    }
}
