#global testerror "Err0036"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
