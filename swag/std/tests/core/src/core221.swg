
using core

#test
{
    @assert(math.isPowerOf2(1024))
    @assert(math.isPowerOf2(512))
    @assert(math.isPowerOf2(4))
    @assert(!math.isPowerOf2(3))
}