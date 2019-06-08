#unittest module module216;
#unittest pass semantic;
using swag;

//#[printByteCode]
func main: ()
{
	if(false)
	{
		@assert(false);
	}
	else
	{
		@assert(true);
	}
	
	@assert(true);
}

#run main;
