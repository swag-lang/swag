#module module129;
#unittest pass semantic;

attr toto() -> var;
attr toto1() -> func;

#unittest error;
#[toto1]
{
	struct T
	{
		#[toto]
		b: bool;
	}
}
