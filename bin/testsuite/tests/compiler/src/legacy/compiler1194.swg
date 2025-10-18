#global testerror "Err0636"

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
