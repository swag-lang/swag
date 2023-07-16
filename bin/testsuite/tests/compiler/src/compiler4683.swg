#global testerror
using Swag;

//#global testerror;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ"
	@assert(b[3] == 0)
}
