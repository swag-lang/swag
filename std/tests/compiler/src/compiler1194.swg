#module "compiler1194"
#unittest error

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
