func makeRepeat16(v: u8)->u16
{
    return (~0'u16 / 255 * v)
}

#test
{
    let a = makeRepeat16(4)
    @assert(a == 1028)
}
