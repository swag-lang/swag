#global testerror "Err0538"
struct A
{
    x: s32
}

impl A
{
    func toto()
    {
    }
}

func aa()
{
    var a: A
    with a
    {
        .toto()
    }
}