/*
    :LICENCE
    MurmurHash3 was written by Austin Appleby, and is placed in the public
    domain. The author hereby disclaims copyright to this source code.
*/

#global namespace Hash
using Swag

#[Safety("", false), Overload]
public func murmur3(h: u32)->u32
{
    var key = h ^ (h >> 16)
    key *= 0x85ebca6b
    key ^= key >> 13
    key *= 0xc2b2ae35
    key ^= key >> 16
    return key
}

#[Safety("", false), Overload]
public func murmur3(h: u64)->u64
{
    var key = h ^ (h >> 33)
    key *= 0xff51afd7ed558ccd
    key ^= key >> 33
    key *= 0xc4ceb9fe1a85ec53
    key ^= key >> 33
    return key
}

#[Safety("", false), Overload]
public func murmur3(a, b: u32)->u32
{
    var na = a * 0xcc9e2d51
    na = Math.ror(na, 17) * 0x1b873593
    var nb = b ^ na
    nb = Math.ror(na, 19) * 5
    return nb + 0xe6546b64
}

#[Safety("", false), Overload]
public func murmur3(a, b: u64)->u64
{
    const K = 0x9ddfea08eb382d69
    var na = (a ^ b) * K
    na ^= (na >> 47)
    var nb = (b ^ na) * K
    nb ^= (nb >> 47)
    return nb * K
}