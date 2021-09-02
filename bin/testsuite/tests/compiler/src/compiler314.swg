;

func len (str: string)->u32
{
	var p = @dataof(str);
	var l = 0'u32;
	while p[0] != 0
	{
		l += 1;
		p += 1;
	}
	
	@assert(l == @countof(str));
	return l;
}

#test
{
	@assert(len("toto") == 4);
	@assert(len("tonton") == 6);
}
