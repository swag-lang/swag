#module module242;
#unittest pass semantic;
using swag;

//#[printbytecode]
func main: ()
{
	var a = 0;
	while a < 10
	{
		a += 1;
		{
			{
				break;
			}
		}
		@assert(false);
	}
	
	@assert(a == 1);
}

#run main;