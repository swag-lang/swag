;
;
using swag;

func main (a: s64)
{
	@assert(a == 666);
}

#run
{
	main(666's64);
}