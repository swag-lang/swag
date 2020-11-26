#import "std"
using std.memory

//#[swag.printbc]
#test
{
    ptr := new'String()
    :ptr = "toto"
    //console.print("%", ptr[0])
    @assert(:ptr == "toto")
    delete(ptr)
}
