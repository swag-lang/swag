func aa(x: u32, y: any)
{

}

namespace aa1650
{
    struct B
    {

    }
}

//#[Swag.printbc]
#run
{
    x := @typeof(aa1650.B)
    y := @typeof(bool)
    if(x == y)
    {
        @assert(false)
    }
}