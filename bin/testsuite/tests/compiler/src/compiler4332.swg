#global testerror "Syn0197"
struct A {}

impl A
{
    func toto()
    {
        var x: func(self: *A)
    }
}