//#[Swag.PrintBc]
#test
{
	a := 10
	c := 20
	res := {&a, &c}
	@assert(dref res.item0 == 10)
	@assert(dref res.item1 == 20)
}
