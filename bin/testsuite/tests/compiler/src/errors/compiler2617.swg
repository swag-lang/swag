#global testerror "Err0201"

#[Swag.NoCopy]
struct AA {}

impl AA
{
    func opPostCopy(me) {}
}
