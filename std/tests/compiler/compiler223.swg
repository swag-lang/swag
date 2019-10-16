;
;
using swag;

var v = 50;

//#[printbc]
func main ()
{
	@assert(v == 50);
	v = 666;
	@assert(v == 666);
}

#run 
{
	main()
}
