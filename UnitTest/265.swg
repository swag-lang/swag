#unittest module module265;
//#unittest pass semantic;
using swag;

func test: (v: s32)->s32
{
	if(v == 1)
		return 1;
	return v * test1(v - 1);
}

//#[printbytecode]
func test1: (v: s32)->s32
{
	if(v == 1)
		return 1;
	return v * test(v - 1);
}

#[test]
func main: ()
{
	@assert(test(5) == 120);
	@assert(test(4) == 24);
}