#global public
using Core

struct ImageMetaData
{
    tag:    Array'u8
    value:  Array'u8
}

impl ImageMetaData
{
    // Returns true if the metadata has the tag 'test'
    mtd isTag(test: const [..] u8)->bool
    {
        if @countof(test) != tag.count:
            return false
        return Memory.compare(@dataof(test), tag.buffer, @countof(test)) == 0
    }

    // Convert the value to a slice
    mtd toSlice()->const [..] u8
    {        
        return value.toSlice()
    }
}