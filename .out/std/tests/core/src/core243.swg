
using Core.Drawing

//#[Swag.PrintBc]
#test
{
	x := Argb.Red
	var y: Color = Argb.Red
    @assert(y.a == 255)
    @assert(y.r == 255)
    @assert(y.g == 0)
    @assert(y.b == 0)
    @assert(y.argb == Argb.Red)
	@assert(@sizeof(Color) == 4)
}