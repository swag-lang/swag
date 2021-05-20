;
;

var a = @["0", "1.0"];
var b = @[1,2,3,4,5,6];

//#[Swag.printbc]
#test
{
	@assert(@countof(a) == 2);
	@assert(@countof(b) == 6);
}
