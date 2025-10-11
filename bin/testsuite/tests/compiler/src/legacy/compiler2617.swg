#global testerror "Err0085"

#[Swag.NoCopy]
struct AA {}

impl AA
{
    func opPostCopy(me) {}
}
