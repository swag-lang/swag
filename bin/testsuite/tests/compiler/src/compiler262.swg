using Swag;

func fact (v: s32)->s32
{
	if(v == 1):
		return 1;
	let res = v * fact(v - 1);
	return res;
}

#test
{
	@assert(fact(5) == 120);
	@assert(fact(4) == 24);
	@assert(fact(3) == 6);
	@assert(fact(2) == 2);
	@assert(fact(1) == 1);
}
