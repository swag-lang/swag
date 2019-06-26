#module module314;

func len: (str: string)->u32
{
	var p = @prop(str, data);
	var l = 0'u32;
	while(p[0] != 0)
	{
		l += 1;
		p += 1;
	}
	
	@assert(l == @prop(str, count));
	return l;
}

#test
{
	@assert(len("toto") == 4);
	@assert(len("tonton") == 6);
}
