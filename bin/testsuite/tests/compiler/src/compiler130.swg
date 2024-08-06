#global testerror "Err0436"

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
