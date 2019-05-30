#unittest module module129
#unittest pass semantic

attr toto: () -> var;
attr toto1: () -> func;

#[toto1]
{
	#[toto]
	var b: bool;
}
