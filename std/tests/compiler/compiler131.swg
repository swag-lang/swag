#module compiler131
#unittest pass semantic;

attr toto() -> var;
#unittest error;
struct TT
{
	#[toto, toto]
	b: bool;
}
