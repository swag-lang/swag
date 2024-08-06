#global testerror "Err0556"

using Swag;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ";
	@assert(b[3] == 0);
}
