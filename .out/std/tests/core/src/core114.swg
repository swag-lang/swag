
using core

struct Pt
{
    x, y: s32
}

impl Hash.IHash32 for Pt
{
    func compute(const self)->u32 => 555
}

#test
{
    var arr: Array'Pt
    arr.add(Pt{1, 1})
    arr.add(Pt{2, 2})
    arr.add(Pt{3, 3})
    @assert(arr.count == 3)

    var pt: Pt
    @assert(Hash.hash32(pt) == 555)
}