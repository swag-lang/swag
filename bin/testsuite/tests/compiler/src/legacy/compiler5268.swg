//#global testerror "Err0635"
func toto(x: u64) {}

#test
{
    var v: u32
    toto(v)
}
