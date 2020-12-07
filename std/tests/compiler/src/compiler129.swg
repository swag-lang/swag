#module "compiler129"
#unittest error
using swag

#[attributeUsage(AttributeUsage.Field)]
attr toto();
#[attributeUsage(AttributeUsage.Function)]
attr toto1();

#[toto1]
{
	struct T
	{
		#[toto]
		b: bool;
	}
}
