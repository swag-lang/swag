#global testerror;

#[attributeUsage(AttributeUsage.StructVariable)]
attr toto();

#[toto]
{
	#[toto]
	struct TT
	{
		b: bool;
	}
}
