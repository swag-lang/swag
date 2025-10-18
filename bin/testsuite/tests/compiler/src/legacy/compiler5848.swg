#global testerror "Err0745"

struct A {}

impl A
{
    #[Swag.Macro]
    func(ptr: bool, back: bool) opVisit(const me, stmt: #code bool) {}
}
