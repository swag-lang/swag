#global testerror "Err0130"

using Swag;

#[Swag.Safety("", true)]
#test
{
	let b = "XYZ";
	@assert(b[3] == 0);
}
