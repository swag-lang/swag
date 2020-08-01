
using swag

#test
{
	array := @[1, 2, 3]
	ptr := &array[0]
	value := deref ptr
	@assert(value == 1)
}
