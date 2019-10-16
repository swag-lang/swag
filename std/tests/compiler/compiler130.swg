#module compiler130
;

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
