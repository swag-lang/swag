#global testerror "Err0475"

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
