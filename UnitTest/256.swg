#unittest module module256;
using swag;

#[Test]
func main: ()
{
	var i = 5;
	while i > 0
	{
		@print("toto\n");
		i -= 1;
	}
}
