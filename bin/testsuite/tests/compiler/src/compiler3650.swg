#global skip
func toto(x: f32, y: bool) {}
func toto(x: bool, y: bool) {}

#test
{
    toto(1.0)
}