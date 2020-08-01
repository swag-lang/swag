;
;

var a = @["0", "1.0"];

//#[swag.printbc]
#test
{
	var b = @["0", "1.0"];
	@assert(@countof(a[0]) == 1);
	@assert(@countof(a[1]) == 3);
	@assert(@countof(b[0]) == 1);
	@assert(@countof(b[1]) == 3);
}
