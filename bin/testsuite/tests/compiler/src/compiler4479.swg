#global testerror "Err0550"
struct A { x: u64; }

func toto() => A{}
#test
{
    if var x = toto()
    {
    }
}