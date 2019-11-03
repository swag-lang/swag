#module compiler131
;

attr toto() -> "var"
#unittest error;
struct TT
{
	#[toto, toto]
	b: bool;
}
