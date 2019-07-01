#module module348;
#unittest pass semantic;

const TOTO = 666;

#[swag.printbytecode]
#test
{
	var b = TOTO;
	@assert(b == 666);
}
