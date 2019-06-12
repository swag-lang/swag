#unittest module module246;
#unittest pass semantic;
using swag;

//#[PrintByteCode]
func main: (a: s32 = 6)
{ 
	@assert(a == 6);
}

//#[PrintByteCode]
func main1: ()
{
	main;
}

#run main1;