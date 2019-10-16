;
;
using swag;

var b0: bool = true;
var b1: u8 = 1;
var b2: u16 = 2;
var b3: u32 = 3;
var b4: u64 = 4;

//#[printbc]
func main ()
{
	@assert(b0 == true);
	@assert(b1 == 1);
	@assert(b2 == 2);
	@assert(b3 == 3);
	@assert(b4 == 4);
}

#run 
{
	main()
}

