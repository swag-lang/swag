
#global testerror "Err0435"
using Swag

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();
#[AttrUsage(AttributeUsage.Function)]
attr toto1();

#[toto1]
{
	struct T
	{
		#[toto]
		b: bool;
	}
}
