var g: [4 * 4] s32 = 5

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    #[Swag.Inline]
    func mp(x0, y0: u64)->&s32 { return &g[0] + (x0 + y0 * 4); }

    let a = #ref mp(1, 1)
    let b = -a
    @assert(b == -5)
    let c = -mp(1, 1)
    @assert(c == -5)
}
