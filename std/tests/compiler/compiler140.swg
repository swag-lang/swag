#module compiler140
#unittest pass semantic;

#unittest error;
attr toto(b: bool) -> var;

#[toto]
namespace A
{
}