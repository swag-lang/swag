#unittest module module256;
#unittest pass backend;
using swag;

#[test]
func main: ()
{
	var i = 5;
	while i > 0
	{
		i -= 1;
	}
	
	@assert(i == 0);
}
