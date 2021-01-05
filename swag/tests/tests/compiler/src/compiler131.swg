#global testerror

using swag

#[attributeUsage(AttributeUsage.Field)]
attr toto();

struct TT
{
	#[toto, toto]
	b: bool;
}
