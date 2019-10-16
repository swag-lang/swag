;
;
using swag;

#run
{
	var a = 1;
	a *= 2;
	@assert(a == 2);
	a *= 2;
	@assert(a == 4);
	
	var b = 1.0'f32;
	b /= 2;
	@assert(b == 0.5);
}