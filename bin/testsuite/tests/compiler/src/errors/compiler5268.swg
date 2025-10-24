//#global testerror "Err0250"
func toto(x: u64) {}

#test
{
    var v: u32
    toto(v)
}
