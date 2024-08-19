#global testerror "Err0509"

using Swag;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ";
	@assert(b[3] == 0);
}
