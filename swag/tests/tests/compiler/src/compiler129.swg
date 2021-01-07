
#global testerror
using swag

#[attributeUsage(AttributeUsage.StructVariable)]
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
