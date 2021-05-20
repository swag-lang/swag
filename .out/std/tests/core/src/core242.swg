
using core.Drawing

#test
{
	var y: Color
    y.setRgbf(1.0, 1.0, 1.0)
    @assert(cast(s64) y.a == 255)
    @assert(cast(s64) y.r == 255)
    @assert(cast(s64) y.g == 255)
    @assert(cast(s64) y.b == 255)
}