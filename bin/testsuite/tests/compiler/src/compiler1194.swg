#global testerror "Err0545"

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
