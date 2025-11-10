#[Swag.EnumFlags]
enum Dir { Z = 0, A = 5 }

var g: [4 * 4] Dir = Dir.A

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    #[Swag.Inline]
    func mp(x0, y0: u64)->&Dir { return &g[0] + (x0 + y0 * 4); }

    let a = #ref mp(1, 1)
    @assert(a & .Z == .Z)
}
