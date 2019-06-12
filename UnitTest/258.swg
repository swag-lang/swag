#unittest module module258;
//#unittest pass backend;
using swag;

func test: (a: s32, b: s32)->s32 
{ 
	return a - b; 
}

#[Test]
func main: ()
{
	@assert(test(60, 40) == 20);
	@assert(test(50, 50) == 0);
	@assert(test(50, 60) == -10);
}
