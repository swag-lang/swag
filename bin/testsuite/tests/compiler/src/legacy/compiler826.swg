//#[Swag.PrintBc]
#test
{
    let x = 256's64
    if x do
        @assert(true)
    else do
        @assert(false)

    let x1 = 25's8
    if x1 do
        @assert(true)
    else do
        @assert(false)

    let x2 = 256's16
    if x2 do
        @assert(true)
    else do
        @assert(false)

    let x3 = 256'u16
    if x3 do
        @assert(true)
    else do
        @assert(false)

    let x4 = 256'u32
    if x4 do
        @assert(true)
    else do
        @assert(false)

    let x5 = 256's32
    if x5 do
        @assert(true)
    else do
        @assert(false)

    let x6 = 25'u8
    if x5 do
        @assert(true)
    else do
        @assert(false)
}
