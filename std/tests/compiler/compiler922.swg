

func average(x: any)
{
	t := cast(*f32) @dataof(x)	
	@assert(t[0] == 10.0)
	//t1 := @typeof(x)
}

//#[swag.printbc]
#test
{
	average(10.0)
}
