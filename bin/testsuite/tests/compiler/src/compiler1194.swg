#global testerror "Syn0047"

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
