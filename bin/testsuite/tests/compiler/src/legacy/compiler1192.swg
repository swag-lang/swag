const A = false

enum E
{
    #if A:
        RED
    #else:
        GREEN
}

#test
{
    var x = E.GREEN
}
