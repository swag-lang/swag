#global testerror "Err0422"

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
