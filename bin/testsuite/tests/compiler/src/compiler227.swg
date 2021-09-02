;
;
using Swag;

//#[PrintBc]
func main ()
{
	var a : s32;
	var b : s32;
	a = 666;
	b = 100;
	@assert(a == 666);
	@assert(b == 100);
	a = 500;
	@assert(a == 500);
	@assert(b == 100);
}

//#[PrintBc]
func main1 ()
{
	main();
}

#run
{
	main1()
}
