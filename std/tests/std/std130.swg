#import "std"
using std

#test
{
    let x: String = "this is a string"

    @assert(!x.startsWidth("This"))
    @assert(x.startsWidth("this"))
    @assert(x.startsWidth("this is a"))
    @assert(x.endsWidth("string"))
    @assert(x.endsWidth("g"))

    @assert(x.startsWidth("this is a string"))
    @assert(x.endsWidth("this is a string"))

    let y: String = "string"
    @assert(x.endsWidth(y.cstr()))
}