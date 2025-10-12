/*
    :LICENCE
    By Bob Jenkins, public domain.
*/

#global namespace Hash
using Swag

#[Safety(.All, .All, false), Overload]
public func jenkins(h: u64)->u64
{
    var key = (h ^ (h >> 30)) * 0xbf58476d1ce4e5b9
    key = (key ^ (key >> 27)) * 0x94d049bb133111eb
    key = key ^ (key >> 31)
    return key
}

#[Safety(.All, .All, false), Overload]
public func jenkins(h: u32)->u32
{
    var key = (h + 0x7ed55d16) + (h << 12)
    key = (key ^ 0xc761c23c) ^ (key >> 19)
    key = (key + 0x165667b1) + (key << 5)
    key = (key + 0xd3a2646c) ^ (key << 9)
    key = (key + 0xfd7046c5) + (key << 3)
    key = (key ^ 0xb55a4f09) ^ (key >> 16)
    return key
}
