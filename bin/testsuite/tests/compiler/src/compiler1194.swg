#global testerror "Err0450"

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
