
using Core.Drawing

//#[Swag.printbc]
#test
{
	var y: Color = "Aqua"
    @assert(y.argb == Argb.Aqua)
    @assert(y == Argb.Aqua)
}