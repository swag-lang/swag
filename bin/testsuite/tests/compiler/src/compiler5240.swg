var g: [4*4] s32 = 5

#[Swag.Optimize("", false)]
//#[Swag.PrintBc]
#test
{
    #[Swag.Inline]
    func mp(x0, y0: u64)->&s32 { return &g[0] + (x0 + y0*4); }

    let a = #ref mp(1, 1)
    @assert(a & 5 == 5)
    @assert(a & 1 == 1)
    @assert(a | 2 == 7)
}