//#[swag.printbc, swag.nooptim]
#test
{
    @assert(10 * 4 - 2 == 38)
    @assert(10 * (4 - 2) == 20)
    @assert(10*2 + 10*2 == 40)
    @assert(10*2 + 10*2 - 10*2 == 20)
    @assert(1<<2*2 == 8)
    @assert(1<<2-1 == 3)
    @assert(1<<3*2 == 16)
    @assert(2-1-1 == 0)
    @assert(2-1+1 == 2)
    @assert(3+2-1 == 4)
    @assert(1*3-3+4/2 == 2)
    @assert(40/2*10 == 200)
}