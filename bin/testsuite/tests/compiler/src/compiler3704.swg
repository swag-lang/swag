#global testerror "Err0550"
func toto()
{
    const A = 1
    var v: u32
    var x: u32 = v ? v : (A - 1)
}