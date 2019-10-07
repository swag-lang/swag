#module module1055

//#[swag.printbc]
#test
{
	x := 0
	ptr := &x
	@free(ptr)
}