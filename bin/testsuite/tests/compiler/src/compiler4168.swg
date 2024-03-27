//#[Swag.PrintBc]
#[Swag.Safety("sanity", true)]
#[Swag.Optim("", false)]
func toto(x: s32)
{
    var a: [4] s32
    while x >= 0
    {
        @assert(a[x] == 0)
    }
}