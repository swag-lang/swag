


//#[Swag.PrintBc]
#test
{
	a := 10
	c := 20
	res := {&a, &c}
	res1 := res
	@assert(dref res1.item0 == 10)
	@assert(dref res1.item1 == 20)
}
