#global testerror "Err0113"
func toto(x: bool, y: f32) {}
func toto(x: bool, y: bool) {}

#test
{
    toto(true)
}