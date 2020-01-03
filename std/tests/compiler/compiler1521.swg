/*struct X
{
    x: s32
    y: bool   
}

#test
{
    var p: X(y: true, x: 0)
    @assert(p.x == 666)
    @assert(p.y == true)
}*/

func toto(x: s32, y: bool)
{

}

#test
{
    toto(y: true, x: 666)
}