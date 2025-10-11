struct(T) Array {}

struct PolyNode
{
    Childs:     Array'*PolyNode
    Parent:     *PolyNode
}

func toto(x: Array'*PolyNode, y: *PolyNode)
{
    @print("toto")
}

#test
{
    var x = #typeof(&toto)
}
