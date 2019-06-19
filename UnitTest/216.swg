#module module216;
#unittest pass semantic;
using swag;

//#[printbytecode]
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
