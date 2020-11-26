#import "std"
using std

//#[swag.printbc]
#test
{
    ptr := memory.new'String()
    ptr[0] = "toto"
    console.print("%", ptr[0])
    memory.delete(ptr)
}
