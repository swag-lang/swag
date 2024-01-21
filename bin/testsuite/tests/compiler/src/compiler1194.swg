#global testerror "Err0546"

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
