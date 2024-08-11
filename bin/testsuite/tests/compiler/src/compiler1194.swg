#global testerror "Err0410"

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
