#global testerror "Err0368"

#[Swag.AttrUsage(Swag.AttributeUsage.StructVariable)]
attr toto();

#[toto]
{
	#[toto]
	struct TT
	{
		b: bool;
	}
}
