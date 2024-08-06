#global testerror "Err0508"

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
