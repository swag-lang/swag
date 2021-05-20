#global public

namespace Hash
{
    interface IHash32
    {
        compute: func(const self)->u32
    }
}

impl Hash.IHash32 for String
{
    func compute(using const self)->u32
    {
        return Hash.Crc32.compute(@mkslice(buffer, length))
    }
}