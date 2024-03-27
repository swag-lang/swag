;
;
using Swag;

//#[PrintBc]
func main ()
{
	var a : s32;
	{
		var b : s32;
		a = 666;
		b = 100;
	}
	@assert(a == 666);
}

#run
{
	main()
}
