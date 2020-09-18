#import "std"
using std

struct Pt
{
    x, y: s32
}

impl std.crypto.IHash32 for Pt
{
    func hash(self)->u32 => 555
}

#test
{
    var arr: Array'Pt
    arr.add(Pt(1, 1))
    arr.add(Pt(2, 2))
    arr.add(@{3, 3})
    @assert(arr.count == 3)

    let pt: Pt
    @assert(std.crypto.hash32(pt) == 555)
}