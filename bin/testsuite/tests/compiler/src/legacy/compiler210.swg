;
;
using Swag;

//#[PrintBc]
func main (a: s32) -> s32
{
	return a + 1;
}

#run
{
	@assert(main(6) == 7);
	@assert(main(4 + 6) == 11);
}