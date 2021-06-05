;
;
using Swag;

//#[printbc]
func main (a: s32 = 6)
{
	@assert(a == 6);
}

//#[printbc]
func main1 ()
{
	main();
}

#run
{
	main1()
}
