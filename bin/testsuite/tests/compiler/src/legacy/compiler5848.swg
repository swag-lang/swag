#global testerror "Err0604"

struct A {}

impl A
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(const me, stmt: #code bool) {}
}
