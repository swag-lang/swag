//#[Swag.PrintBc]
#test
{
	c := @[666,667]
	T(&c[1])
	@assert(c[0] == 666)
	@assert(c[1] == 668)
}

//#[Swag.PrintBc]
func T(param: *s32)
{
	param[0] += 1
}
