#global public
using Core

struct ImageMetaData
{
    sig:       u32
    tag:       Array'u8
    value:     Array'u8
}

impl ImageMetaData
{
    // Returns true if the metadata has the tag 'val'
    mtd isTag(val: const [..] u8)->bool
    {
        if @countof(val) != .tag.count do
            return false
        return Memory.compare(@dataof(val), .tag.buffer, @countof(val)) == 0
    }

    // Convert the value to a slice
    mtd toSlice()->const [..] u8
    {
        return .value.toSlice()
    }
}
