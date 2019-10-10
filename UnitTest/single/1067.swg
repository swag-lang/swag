#module module1067

#[swag.printbc]
func toString() -> [..] u8
{
	return [0, 1, 2, 3]
}

#test
{
	var x = toString()
	@assert(x[0] == 0)
}