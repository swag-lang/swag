#global testerror "Err0417"

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
