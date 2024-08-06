#global testerror "Err0437"

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
