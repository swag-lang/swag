using swag;

//#[printbc]
func main ()
{
	var a = 10;
	while a > 0
	{
		a -= 1;
		if(a == 5) break;
		continue;
	}
	@assert(a == 5);
}

#run
{
	main()
}
