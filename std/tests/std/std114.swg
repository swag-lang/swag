#import "std"
using std.collections, std.text, std.diagnostics

#test
{
    struct Pt
    {
        x, y: s32
    }

    var arr: Array'Pt
    arr.add(Pt(1, 1))
    arr.add(Pt(2, 2))
    arr.add(@{3, 3})
    @assert(arr.count == 3)
}