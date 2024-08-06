#global testerror "Err0059"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
