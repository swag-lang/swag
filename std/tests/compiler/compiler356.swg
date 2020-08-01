;
;

var a = @["0", "1.0"];

//#[swag.printbc]
#test
{
	a[1] = "toto";
	@assert(a[1] == "toto");
	a[1] = "1.0";
}
