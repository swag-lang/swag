
using Swag

//#[Swag.printbc]
#test
{
	ptr := @typeof(const *TypeInfo)

	ptr1 := cast(const *TypeInfoArray) @typeof([2] s32)
	@assert(ptr1.count == 2)
	@assert(ptr1.pointedType == @typeof(s32))

	ptr2 := cast(const *TypeInfoArray) @typeof([2, 3] string)
	@assert(ptr2.count == 2)
	@assert(ptr2.totalCount == 6)
	@assert(ptr2.pointedType == @typeof([3] string))
	@assert(ptr2.finalType == @typeof(string))

	ptr3 := cast(const *TypeInfoSlice) @typeof([..] string)
	@assert(ptr3.pointedType == @typeof(string))
}
