#global testerror "Err0500"

using Swag;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ";
	@assert(b[3] == 0);
}
