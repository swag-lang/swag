#module "compiler248"
;
using swag;

//#[printbc]
func main (a: s32 = 6)->s32
{ 
	//@print("1");
	return a;
}

#unittest error;
func main (a: s32, b: s32 = 0)->s32
{ 
	//@print("2");
	return b;
}

func main1 ()
{
	@assert(main() == 6);
	@assert(main(5) == 5);
	@assert(main(5, 7) == 7);
}

#run 
{
	main1()
}
