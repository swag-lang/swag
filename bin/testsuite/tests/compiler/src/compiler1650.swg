func aa(x: u32, y: any) {}

namespace aa1650
{
    struct B {}
}

//#[Swag.PrintBc]
#run
{
    let x = #typeof(aa1650.B)
    let y = #typeof(bool)
    if (x == y)
    {
        @assert(false)
    }
}
