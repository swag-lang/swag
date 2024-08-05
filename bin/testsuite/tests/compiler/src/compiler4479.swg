#global testerror "Err0644"
struct A { x: u64; }

func toto() => A{}
#test
{
    if var x = toto()
    {
    }
}