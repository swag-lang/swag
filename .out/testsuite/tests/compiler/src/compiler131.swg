#global testerror

using Swag

#[attributeUsage(AttributeUsage.StructVariable)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
