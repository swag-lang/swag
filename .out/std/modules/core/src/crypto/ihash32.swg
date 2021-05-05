#global public

namespace hash
{
    interface IHash32
    {
        compute: func(const self)->u32
    }
}

impl hash.IHash32 for String
{
    func compute(using const self)->u32
    {
        return hash.Crc32.compute(@mkslice(buffer, length))
    }
}