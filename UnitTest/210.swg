#unittest module module210;
#unittest pass semantic;
using swag;

func main: (a: s32) -> s32
{
	return a + 1;
}

#run
{
	@assert(main(6) == 7);
}