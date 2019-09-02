#module module357;
//#unittest pass semantic;

var a = ["0", "1.0"];
var b = [1,2,3,4,5,6];

//#[swag.printbc]
#test
{
	@assert(@op(a, count) == 2);
	@assert(@op(b, count) == 6);
}
