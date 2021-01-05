#global testerror;

#[attributeUsage(AttributeUsage.Field)]
attr toto();

#[toto]
{
	#[toto]
	struct TT
	{
		b: bool;
	}
}
