#global testerror "Err0561"

struct A {}

impl A
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(const me, stmt: #code bool) {}
}
