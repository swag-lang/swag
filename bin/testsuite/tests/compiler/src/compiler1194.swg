#global testerror "Err1068"

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
