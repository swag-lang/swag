#global testerror "Err0561"
using Swag

struct String
{
    x: string
}

impl String
{
    #[Implicit]
    mtd opAffect(value: string)
    {
        me.x = value
    }
}

// This is a generic dynamic array.
struct(T) Array
{
    buffer:        ^T             // Memory block of all datas
    count:         u64            // Number of valid datas
    capacity:      u64            // Number of elements that can be stored in the buffer
}

impl Array
{
    #[Inline]
    mtd opIndexAffect(index: u64, value: T)
    {
        buffer[index] = value
    }
}

#test
{
    var t: [10] String
    var a: Array'String
    a.buffer = &t[0]

    a[0] = "toto"
}