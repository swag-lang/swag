

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

	var t = @typeof(TOTO)
	var ptr = cast(const *TypeInfoEnum) t
	@assert(@countof(ptr.values) == 3)

	var ptr1 = ptr.values[1]
	@assert(ptr1.name == "G")

	var ptrVal = cast(const *s32) ptr1.value;
	@assert(dref ptrVal == 200)
}