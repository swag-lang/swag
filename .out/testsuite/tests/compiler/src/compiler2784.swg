struct AA
{

}

var G = false
func toto(x: AA)
{
    G = true
}

//#[Swag.PrintBc]
#test
{
    x := &toto
    var tt: AA
    tt.x()
}