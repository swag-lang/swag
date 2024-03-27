struct AA
{
    vv = 6
}

impl AA
{
    func opCmp(using self, x: s32)->s32
    {
        return vv <=> x
    }
}

#test
{
    var x: AA
    switch x
    {
    case 10 to 5:  break
    default:       @assert(false)
    }

    switch x
    {
    case 0 to 5:   @assert(false)
    case 10 to 6:  break
    default:       @assert(false)
    }
}
