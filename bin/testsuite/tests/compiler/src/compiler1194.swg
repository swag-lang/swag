#global testerror "Syn0048"

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
