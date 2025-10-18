#global testerror "Err0641"
struct A
{
    x:   s32
}

impl A
{
    func toto() {}
}

func aa()
{
    var a: A
    with a
    {
        .toto()
    }
}
