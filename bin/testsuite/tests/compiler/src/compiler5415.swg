var g = 56

func toto()->&s32
{
    return &g
}

#[Swag.Optimize("", false)]
#test
{
    let t = 0
    let c = t orelse toto()
    @assert(c == 56)
}