#module "compiler1720"
#unittest error

#test
{
    a := 0
    switch a
    {
        case 0:
        case 1: break
    }
}