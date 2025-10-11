#test
{
    var x, y = 2
    let xr = #ref &x
    let yr = #ref &y
    @assert(xr + yr == 4)
    @assert(xr - yr == 0)
    @assert(xr * yr == 4)
    @assert(xr / yr == 1)
    @assert(xr % yr == 0)
    @assert(xr | yr == 2)
    @assert(xr & yr == 2)
    @assert(-xr == -2)
    @assert(~xr == ~2)
    @assert(!xr == false)
}
