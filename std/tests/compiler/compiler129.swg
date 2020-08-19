#module "compiler129"
using swag

#[attributeUsage(AttributeUsage.Field)]
attr toto();
#[attributeUsage(AttributeUsage.Function)]
attr toto1();

#unittest error;
#[toto1]
{
	struct T
	{
		#[toto]
		b: bool;
	}
}
