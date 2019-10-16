#module compiler130
#unittest pass semantic;

attr toto() -> var;

#unittest error;
#[toto]
{
	#[toto]
	struct TT
	{
		b: bool;
	}
}
