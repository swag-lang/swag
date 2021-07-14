

func concrete()->s32
{
	return 666
}

func concrete1()->f32
{
	return 666
}

//#[Swag.PrintBc]
func(T) toto(x: func()->T)->T
{
	return x()
}

#test
{
	lambda := &concrete
	result := toto(lambda)
	@assert(result == 666)

	lambda1 := &concrete1
	result1 := toto(lambda1)
	@assert(result1 == 666.0)
}
