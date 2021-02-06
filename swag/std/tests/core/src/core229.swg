#import "core"
using core.utf16

var t: [] struct {
    in:  [..] char
    out: [..] u16
} = @[
    {[1,2,3,4], [1,2,3,4]},
]

//#[swag.printbc]
#test

{
    visit *p: t
    {
        console.print("%", @countof(p.in))
        //r := fromCharArray(p.in)
    }
}