

func average(x: any)
{
	t := cast(*f32) @dataof(x)
	@assert(dref t == 10.0)
	//t1 := @typeof(x)
}

//#[Swag.PrintBc]
#test
{
	average(10.0)
}
