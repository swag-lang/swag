#global testerror "Err0425"

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
