

var g: s32 = 666
var gf: f32 = 666

func concrete()->*s32
{
	return &g
}

func concrete1()->*f32
{
	return &gf
}

//#[Swag.PrintBc]
func(T) toto(x: func()->*T)->T
{
	var ptr = x()
	return dref ptr
}

#test
{
	var lambda = &concrete
	var result = toto(lambda)
	@assert(result == 666)
	//@print(result)

	var lambda1 = &concrete1
	var result1 = toto(lambda1)
	@assert(result1 == 666)
	//@print(result1)
}
