

//#[Swag.PrintBc]
#test
{
	a := 0
	defer @assert(a == 1)
	a += 1
}