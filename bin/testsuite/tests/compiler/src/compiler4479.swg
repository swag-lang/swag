#global testerror "Err0600"
struct A { x: u64; }

func toto() => A{}
#test
{
    if var x = toto()
    {
    }
}