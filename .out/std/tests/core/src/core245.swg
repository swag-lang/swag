
using Core.Drawing

//#[Swag.PrintBc]
#test
{
	var y: Color = "Aqua"
    @assert(y.argb == Argb.Aqua)
    @assert(y == Argb.Aqua)
}