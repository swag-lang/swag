#global testerror "Err0013"
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
