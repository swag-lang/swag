using core.Drawing

//#[Swag.printbc]
#test
{
	var y: Color = 0xFFFF0000
    @assert(y.toArgbf().a == 1)
    @assert(y.toArgbf().r == 1)
    @assert(y.toArgbf().g == 0)
    @assert(y.toArgbf().b == 0)
}