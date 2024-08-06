
#global testerror "Err0471"
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
