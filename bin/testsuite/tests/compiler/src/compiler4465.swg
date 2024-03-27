#test
{
    var a   = [`1`'u8]
    let ptr = &a[0]
    let x   = dref ptr
    @assert(x - `0`'u8 == 1)
}

#test
{
    var a   = [`1`'u16]
    let ptr = &a[0]
    let x   = dref ptr
    @assert(x - `0`'u16 == 1)
}
