#import "std"
using std

struct Pt
{
    x, y: s32
}

impl hash.IHash32 for Pt
{
    func compute(self)->u32 => 555
}

#test
{
    var arr: Array'Pt
    arr.add(Pt{1, 1})
    arr.add(Pt{2, 2})
    arr.add(@{3, 3})
    @assert(arr.count == 3)

    let pt: Pt
    @assert(hash.hash32(pt) == 555)
}