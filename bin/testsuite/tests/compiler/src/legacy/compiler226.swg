;
;
using Swag;

//#[PrintBc]
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

//#[PrintBc]
func main1 ()
{
	main();
}

#run
{
	main1()
}
