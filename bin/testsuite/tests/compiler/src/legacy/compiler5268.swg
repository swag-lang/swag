//#global testerror "Err0198"
func toto(x: u64) {}

#test
{
    var v: u32
    toto(v)
}
