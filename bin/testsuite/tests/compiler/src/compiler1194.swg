#global testerror "Err0519"

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
