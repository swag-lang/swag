
using Swag

#test
{
	var array = [1, 2, 3]
	var ptr = &array[0]
	var value = dref ptr
	@assert(value == 1)
}
