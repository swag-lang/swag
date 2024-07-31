#global testerror "Err0549"

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
