using Core

#test
{
    @assert(Math.roundUpToPowerOf2(5'u32) == 8)
    @assert(Math.roundDownToPowerOf2(1505'u32) == 1024)
    @assert(Math.roundUpToPowerOf2(1505'u32) == 2048)
}
