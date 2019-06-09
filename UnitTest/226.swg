#unittest module module226;
#unittest pass semantic;
using swag;

#[printByteCode]
func main: ()
{
	var a : s32;
	{
		var b : s64;
	}
	{
		var b : s64;
	}	
}

//#[printByteCode]
func main1: ()
{
	main();
}

#run main1;
