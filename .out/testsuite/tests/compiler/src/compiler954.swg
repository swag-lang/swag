
using Swag

func toto()->s32 => 666
func toto1()->s32 => 667

//#[Swag.PrintBc]
#test
{
	ptr := &toto
	ptr1 := &toto
	@assert(ptr == ptr1)
	ptr1 = &toto1
	@assert(ptr != ptr1)
}
