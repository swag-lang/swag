#global testerror "Err0055"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
