#unittest module module256;
using swag;

#[Test]
func main: ()
{
	@assert(true);
	var i = 5;
	while i > 0
	{
		@print("toto\n");
		i -= 1;
	}
}
