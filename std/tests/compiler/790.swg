

enum TOTO : string
{
	R = "str100"
	G = "str200"
	B = "str300"
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
	
	ptrVal := cast(*string) ptr1.value;
	@assert(ptrVal[0] == "str200")
}