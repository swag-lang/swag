#global testerror "Err0636"
struct A { x: u64; }

func toto() => A{}
#test
{
    if var x = toto()
    {
    }
}