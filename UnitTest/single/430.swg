#module module430
//#unittest pass semantic

//#[swag.printbytecode]
#test
{
	len("toto");
	len("coucou")
	len({1'u8,2,3,4,5,5,5,5,5,5})
}

func len(buffer: const [..] u8)
{
	result := 0'u32
	loop(buffer)
		result += 1;
	@assert(result == @prop(buffer, count));
}
