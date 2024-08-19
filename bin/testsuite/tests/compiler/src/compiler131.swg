#global testerror "Err0039"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
