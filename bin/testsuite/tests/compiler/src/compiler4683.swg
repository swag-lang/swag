#global testerror "Err0136"
using Swag;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ"
	@assert(b[3] == 0)
}
