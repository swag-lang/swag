#global testerror "Err0562"

using Swag;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ";
	@assert(b[3] == 0);
}
