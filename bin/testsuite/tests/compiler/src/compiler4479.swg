#global testerror "Err0562"
struct A { x: u64; }

func toto() => A{}
#test
{
    if var x = toto()
    {
    }
}