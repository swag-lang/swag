


func(T) valueof(x: any)->T
{
	ptr := cast(*T) @dataof(x)
	return ptr[0]
}

//#[Swag.printbc]
func toto(xx: string, a: ...)
{
	x0 := valueof's32(a[0])
	@assert(x0 == 10)
	x1 := valueof'string(a[1])
	@assert(x1 == "toto")
	x2 := valueof's32(a[2])
	@assert(x2 == 600)
}

//#[Swag.printbc]
#test
{
	toto("true", 10, "toto", 600)
}
