struct MyStruct
{
    x:     s32 = 666
    y:     string = "454"
}

const X1 = MyStruct{50, "value"}
#assert(X1.x == 50)
#assert(X1.y == "value")

#test
{
    const X2 = MyStruct{50, "value"}
    #assert(X2.x == 50)
    #assert(X2.y == "value")
}
