func toto() -> {f32, bool}
{
	return @{666, true}
}

#test
{
	var x: f32
	var y: bool

	(x, y) = toto()
    @assert(x == 666)
    @assert(y == true)
}