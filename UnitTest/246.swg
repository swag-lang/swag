#unittest module module246;
#unittest pass semantic;
using swag;

#[printByteCode]
func main: (a: s32 = 6)
{ 
	@assert(a == 6);
}

#[printByteCode]
func main1: ()
{
	main;
}

#run main1;