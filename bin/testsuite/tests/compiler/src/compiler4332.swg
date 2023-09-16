#global testerror "Err1197"
struct A {}

impl A
{
    func toto()
    {
        var x: func(self: *A)
    }
}