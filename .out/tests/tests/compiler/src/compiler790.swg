

enum TOTO : string
{
	R = "str100"
	G = "str200"
	B = "str300"
}

//#[Swag.printbc]
#test
{
	using Swag

	t := @typeof(TOTO)
	ptr := cast(const *TypeInfoEnum) t
	@assert(@countof(ptr.values) == 3)

    @assert(ptr.values[0].name == "R")
    @assert(ptr.values[1].name == "G")

	ptr1 := ptr.values[1]
	@assert(ptr1.name == "G")

	ptrVal := cast(const *string) ptr1.value;
	@assert(ptrVal[0] == "str200")
}