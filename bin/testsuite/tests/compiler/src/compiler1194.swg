#global testerror "Err0473"

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
