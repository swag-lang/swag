#module module246;
#unittest pass semantic;
using swag;

//#[printbytecode]
func main (a: s32 = 6)
{ 
	@assert(a == 6);
}

//#[printbytecode]
func main1 ()
{
	main;
}

#run main1;