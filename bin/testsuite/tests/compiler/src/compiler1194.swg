#global testerror "Err0456"

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
