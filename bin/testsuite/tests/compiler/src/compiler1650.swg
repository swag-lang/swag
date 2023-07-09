func aa(x: u32, y: any)
{

}

namespace aa1650
{
    struct B
    {

    }
}

//#[Swag.PrintBc]
#run
{
    var x = @typeof(aa1650.B)
    var y = @typeof(bool)
    if(x == y)
    {
        @assert(false)
    }
}