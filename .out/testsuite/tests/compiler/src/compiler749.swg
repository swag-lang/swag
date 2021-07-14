
/*
#[Swag.PrintBc]
func lambda(x: f32)
{
	@print(x)
}

#[Swag.PrintBc]
func(T) toto(x: (T))
{
	x(666)
}

//#[Swag.PrintBc]
#test
{
	toto(&lambda)
}*/

func toto(x: f32)
{
	@assert(x == 666)
}

#test
{
	toto(666)
}