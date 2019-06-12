#unittest module module226;
#unittest pass semantic;
using swag;

//#[PrintByteCode]
func main: ()
{
	var a : s32;
	{
		var b : s64;
		var c : s64;
	}
	{
		var b : s64;
	}	
}

//#[PrintByteCode]
func main1: ()
{
	main();
}

#run main1;
