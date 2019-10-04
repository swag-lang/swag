#module module1015
using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(const *typeinfo)
	
	ptr1 := cast(const *typeinfo_array) @typeof([2] s32)
	@assert(ptr1.count == 2)
	@assert(ptr1.pointed_type == @typeof(s32))
	
	ptr2 := cast(const *typeinfo_array) @typeof([2, 3] string)
	@assert(ptr2.count == 2)
	@assert(ptr2.totalCount == 6)
	@assert(ptr2.pointed_type == @typeof([3] string))
	@assert(ptr2.final_type == @typeof(string))
}
