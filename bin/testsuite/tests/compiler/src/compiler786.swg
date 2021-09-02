

enum TOTO
{
	R = 100
	G = 200
	B = 300
}

//#[Swag.PrintBc]
#test
{
	using Swag

	t := @typeof(TOTO)
	ptr := cast(const *TypeInfoEnum) t
	@assert(@countof(ptr.values) == 3)

	ptr1 := ptr.values[1]
	@assert(ptr1.name == "G")

	ptrVal := cast(const *s32) ptr1.value;
	@assert(ptrVal[0] == 200)
}