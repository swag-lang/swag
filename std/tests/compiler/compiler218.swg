;
;
using swag;

var g: s64 = 5;
var h = 5555;
var i = 1.5;

#run
{
	@assert(g == 5);
	@assert(h == 5555);
	@assert(i == 1.5);
}