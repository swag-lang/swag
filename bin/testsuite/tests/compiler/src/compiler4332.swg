#global testerror "Err0684"
struct A {}

impl A
{
    func toto()
    {
        var x: func(self: *A)
    }
}