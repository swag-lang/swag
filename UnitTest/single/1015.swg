#module module1015
using swag

//#[swag.printbc]
#test
{
	ptr := @typeof(const *Typeinfo)
	
	ptr1 := cast(const *TypeinfoArray) @typeof([2] s32)
	@assert(ptr1.count == 2)
	@assert(ptr1.pointed_type == @typeof(s32))
	
	ptr2 := cast(const *TypeinfoArray) @typeof([2, 3] string)
	@assert(ptr2.count == 2)
	@assert(ptr2.totalCount == 6)
	@assert(ptr2.pointed_type == @typeof([3] string))
	@assert(ptr2.final_type == @typeof(string))
	
	ptr3 := cast(const *TypeinfoSlice) @typeof([..] string)
	@assert(ptr3.pointed_type == @typeof(string))
}
