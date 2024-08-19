#global testerror "Err0380"

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
