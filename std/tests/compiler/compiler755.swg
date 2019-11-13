

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

//#[swag.printbc]
func(T) toto(x: func()->*T)->T
{
	ptr := x()
	return ptr[0]
}

#test
{	
	lambda := &concrete
	result := toto(lambda)
	@assert(result == 666)
	//@print(result)
	
	lambda1 := &concrete1
	result1 := toto(lambda1)
	@assert(result1 == 666)
	//@print(result1)	
}
