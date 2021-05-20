;
;
using Swag;

//#[printbc]
func main ()
{
	var a : s32;
	{
		var b : s64;
		var c : s64;
	}
	{
		var b : s64;
	}
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
