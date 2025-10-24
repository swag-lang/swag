#global testerror "Err0174"
using Swag

struct Array
{
    count:     u32
    size:      u32
}

impl Array
{
    mtd opDrop()
    {
        var x = &count
    }
}
