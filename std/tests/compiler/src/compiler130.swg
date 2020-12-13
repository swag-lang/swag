
#[attributeUsage(AttributeUsage.Field)]
attr toto();

#unittest error;
#[toto]
{
	#[toto]
	struct TT
	{
		b: bool;
	}
}
