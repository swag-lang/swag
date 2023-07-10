//#[Swag.PrintBc]
#[Swag.Safety("sanity", true)]
func toto()
{
    var a: [4] s32

    let x = -1
    while x >= 0
    {
        @assert(a[x] == 0)
    }
}