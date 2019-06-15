#unittest module module257;
//#unittest pass backend;
func test: ()->s32 { return 666; }

#[swag.test]
func main: ()
{
	@assert(test == 666);
}
