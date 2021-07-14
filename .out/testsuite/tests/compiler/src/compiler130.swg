#global testerror;

#[AttrUsage(AttributeUsage.StructVariable)]
attr toto();

#[toto]
{
	#[toto]
	struct TT
	{
		b: bool;
	}
}
