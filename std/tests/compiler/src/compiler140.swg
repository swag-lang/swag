#module "compiler140"
;

#unittest error;
attr toto(b: bool) -> var;

#[toto]
namespace A
{
}