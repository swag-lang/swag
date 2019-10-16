;
;
using swag;

func main (a: f64, b: f64)
{
	@assert(a == 666);
	@assert(b == 555);
}

#run 
{
	main(666'f64, 555'f64);
}