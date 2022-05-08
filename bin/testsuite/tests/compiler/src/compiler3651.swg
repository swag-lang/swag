#global testerror
func toto(x: bool, y: f32) {}
func toto(x: bool, y, z: bool) {}

#test
{
    toto(true, true)
}