#global testerror "Syn0068"

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
