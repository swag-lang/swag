//#[swag.printbc]
#test
{
	var c = 0xFFFFFFFF_FFFFFFFF;
	var a = c >> 56;
    //@print(cast(s64) a)
	@assert(a == 0xFF);
}
