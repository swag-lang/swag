//#[swag.printbc]
#test
{
	x := [1, 2, 3]
	ptr := &x
	count := 3
	init(ptr, count)
	@print(cast(s64) x[0])
	@print(cast(s64) x[1])
	@print(cast(s64) x[2])
}