#import "std"
using std.memory

//#[swag.printbc]
#test
{
    {
        ptr := new'String()
        :ptr = "toto"
        @assert(:ptr == "toto")
        delete(ptr)
    }

    {
        ptr := newArr'String(10)
        loop 10 ptr[@index] = "toto"
        deleteArr(ptr, 10)
    }
}
