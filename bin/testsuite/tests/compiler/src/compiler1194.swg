#global testerror "Err0423"

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
