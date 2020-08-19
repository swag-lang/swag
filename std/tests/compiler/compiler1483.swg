#module "compiler1483"
#unittest error

#test
{    
    ptr := @dataof("xxx")
    ptr[0] = 0
}