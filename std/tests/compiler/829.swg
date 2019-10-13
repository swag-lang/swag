#module module829

//#[swag.printbc]
#test
{
	a := 0
	defer @assert(a == 1)
	a += 1
}