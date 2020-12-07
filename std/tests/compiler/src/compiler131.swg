#module "compiler131"
using swag

#[attributeUsage(AttributeUsage.Field)]
attr toto();

#unittest error
struct TT
{
	#[toto, toto]
	b: bool;
}
