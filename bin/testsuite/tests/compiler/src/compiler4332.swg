#global testerror "Err0663"
struct A {}

impl A
{
    func toto()
    {
        var x: func(me: *A)
    }
}