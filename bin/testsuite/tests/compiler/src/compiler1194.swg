#global testerror "Err0713"

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
