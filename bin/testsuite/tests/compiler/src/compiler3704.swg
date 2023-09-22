#global testerror "Err0177"
func toto()
{
    const A = 1
    var v: u32
    var x: u32 = v ? v : (A - 1)
}