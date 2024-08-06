#global testerror "Err0474"

const A = false

enum E
{
    #if A
    {
        RED
    }
    #else
    {
        GREEN
    }
