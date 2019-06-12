#unittest module module257;
#unittest pass backend;
using swag;

func test: ()->s32 { return 666; }

#[Test]
func main: ()
{
	@assert(test == 666);
}
