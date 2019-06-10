#unittest module module247;
#unittest pass semantic;
using swag;

//#[printByteCode]
func main: (a: s32 = 6)->s32
{ 
	return a;
}
func main: (a: s32, b: s32)->s32
{ 
	return b;
}

func main1: ()
{
	main;
}

#run main1;