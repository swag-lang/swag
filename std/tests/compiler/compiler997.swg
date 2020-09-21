
using swag

#test
{
	array := @[1, 2, 3]
	ptr := &array[0]
	value := :ptr
	@assert(value == 1)
}
