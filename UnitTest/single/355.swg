#module module355;
//#unittest pass semantic;

var a = ["0", "1.0"];

//#[swag.printbc]
#test
{
	var b = ["0", "1.0"];
	@assert(@op(a[0], count) == 1);
	@assert(@op(a[1], count) == 3);
	@assert(@op(b[0], count) == 1);
	@assert(@op(b[1], count) == 3);
}
