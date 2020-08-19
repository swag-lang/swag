#module "compiler1721"
#unittest error

#test
{
    alias a = s32
    {
        func A()
        {
            alias a = s32
        }
    }
}