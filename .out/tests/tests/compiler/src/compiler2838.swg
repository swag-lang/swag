//#[swag.printbc]
#test
{
	//const ExponentBias = 1023
    //exp10 := -18's32
    //clz := 4'u64
	retExp2 := cast(u64) ((((217706's64 *% -18's32) >>% 16) | bitcast(s64) 0xFFFF000000000000) + 64 + 1023) - 4
    @assert(retExp2 == 1023)
}