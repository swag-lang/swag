#global testerror "Err0411"

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
