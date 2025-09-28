;
;
using Swag;

var b0: bool = true;
var b1: u64 = 666;
var b2: f32 = 1.5;

//#[PrintBc]
func main ()
{
	@assert(b0 == true);
	b0 = false;
	@assert(b0 == false);

	@assert(b1 == 666);
	b1 = 50;
	@assert(b1 == 50);

	@assert(b2 == 1.5);
	b2 = 1 + 3;
	@assert(b2 == 4.0);
}

#run
{
	main()
}
