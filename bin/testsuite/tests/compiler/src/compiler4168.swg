//#[Swag.PrintBc]
#[Swag.Sanity(true), Swag.Optimize(.All, false)]
func toto(x: s32)
{
    var a: [4] s32
    while x >= 0
    {
        @assert(a[x] == 0)
    }
}
