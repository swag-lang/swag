#global testerror "Err0401"

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
