#global testerror
func toto(x: bool, y: f32) {}
func toto(x: bool, y: bool) {}

#test
{
    toto(true)
}