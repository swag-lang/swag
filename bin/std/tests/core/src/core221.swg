using Core

#test
{
    @assert(Math.isPowerOf2(1024'u32))
    @assert(Math.isPowerOf2(512'u32))
    @assert(Math.isPowerOf2(4'u32))
    @assert(!Math.isPowerOf2(3'u32))
}
