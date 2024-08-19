#global testerror "Err0559"
struct A { x: u64; }

func toto() => A{}
#test
{
    if var x = toto()
    {
    }
}