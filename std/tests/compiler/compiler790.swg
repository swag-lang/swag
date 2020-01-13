

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
	ptr := cast(const *TypeInfoEnum) t
	@assert(@countof(ptr.values) == 3)
	
    @assert(ptr.values[0].name == "R")
    @assert(ptr.values[1].name == "G")

	ptr1 := ptr.values[1]
	@assert(ptr1.name == "G")
	
	ptrVal := cast(*string) ptr1.value;
	@assert(ptrVal[0] == "str200")
}