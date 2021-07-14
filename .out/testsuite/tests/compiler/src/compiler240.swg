#global testerror;
;
using Swag;

//#[PrintBc]
func main ()
{
	var a = 10;
	//while a > 0
	{
		a -= 1;
		if a == 5 break;
	}
}

#run
{
	main()
}
