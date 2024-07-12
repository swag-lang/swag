struct AA
{
    vv = 6
}

impl AA
{
    //#[Swag.Inline]
    func opCmp(using self, x: s32)->s32
    {
        return vv <=> x
    }
}

#test
{
    var x: AA
    var a = 0
    switch x
    {
    case 10 to 5:  a = 1; break
    default:       @assert(false)
    }

    @print(a)
}
