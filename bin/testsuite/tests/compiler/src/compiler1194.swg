#global testerror "Err0465"

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
