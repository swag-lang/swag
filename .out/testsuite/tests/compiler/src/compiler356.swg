;
;

var a = @["0", "1.0"];

//#[Swag.PrintBc]
#test
{
	a[1] = "toto";
	@assert(a[1] == "toto");
	a[1] = "1.0";
}
