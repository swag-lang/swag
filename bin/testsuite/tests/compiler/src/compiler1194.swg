#global testerror "Err0428"

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
