;
;
using Swag;

//#[PrintBc]
func main (a: s32) -> s32
{
	return a + 1;
}

//#[PrintBc]
func main1 ()
{
	@assert(main(6) == 7);
	@assert(main(4 + 6) == 11);
}

#run
{
	main1();
}
