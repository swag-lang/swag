#global testerror "Err0533"

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
