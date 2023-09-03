#global testerror "Err0591"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
