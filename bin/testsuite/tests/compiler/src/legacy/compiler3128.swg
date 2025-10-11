struct MyStruct
{
    x:     s32 = 666
    y:     string = "454"
}

const X1 = MyStruct{y: "value"}
#assert(X1.y == "value")
