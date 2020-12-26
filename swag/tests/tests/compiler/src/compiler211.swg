;
;
using swag;

func main (a: s32) -> s32
{
	return a + 1;
}


#run
{
	@assert(main(main(6)) == 8);
	@assert(main(main(main(6))) == 9);
}