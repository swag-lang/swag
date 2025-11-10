const A = false

enum E
{
    #if A do
        RED
    #else do
        GREEN
}

#test
{
    var x = E.GREEN
}
