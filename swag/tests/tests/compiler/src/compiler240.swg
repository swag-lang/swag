
;
using swag;

//#[printbc]
#unittest error;
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
