
using Core

#test
{
    @assert(Math.isPowerOf2(1024))
    @assert(Math.isPowerOf2(512))
    @assert(Math.isPowerOf2(4))
    @assert(!Math.isPowerOf2(3))
}