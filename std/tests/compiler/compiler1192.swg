const A = false

enum E
{
#if A
    RED
#else
    GREEN
}

#test
{
    x := E.GREEN
}