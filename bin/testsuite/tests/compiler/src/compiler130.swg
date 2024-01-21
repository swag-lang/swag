#global testerror "Err0492"

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
