#module module355;
//#unittest pass semantic;

var a = ["0", "1.0"];

//#[swag.printbc]
#test
{
	var b = ["0", "1.0"];
	@assert(@prop(a[0], count) == 1);
	@assert(@prop(a[1], count) == 3);
	@assert(@prop(b[0], count) == 1);
	@assert(@prop(b[1], count) == 3);
}
