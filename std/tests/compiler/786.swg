

enum TOTO
{
	R = 100
	G = 200
	B = 300
}

//#[swag.printbc]
#test
{
	using swag
	
	t := @typeof(TOTO)
	ptr := cast(const *TypeinfoEnum) t
	@assert(@countof(ptr.values) == 3)
	
	ptr1 := ptr.values[1]
	@assert(ptr1.namedParam == "G")
	
	ptrVal := cast(*s32) ptr1.value;
	@assert(ptrVal[0] == 200)
}