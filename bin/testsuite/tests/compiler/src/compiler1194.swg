#global testerror "Err0415"

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
