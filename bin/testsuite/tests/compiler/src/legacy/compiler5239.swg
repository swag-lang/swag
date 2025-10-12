var g: [4 * 4] s32 = 5

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    #[Swag.Inline]
    func mp(x0, y0: u64)->&s32 { return &g[0] + (x0 + y0 * 4); }

    var a: s32
    //@breakpoint()
    a = mp(1, 1)
    @assert(a == 5)

    mp(1, 1) = 2
    a        = mp(1, 1)
    @assert(a == 2)
}
