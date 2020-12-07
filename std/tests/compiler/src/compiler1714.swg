#module "compiler1714"
#unittest error

#test
{
    {
        alias point = string
        func toto(x: point)
        {
            alias point = s32
            var y: point
        }

        toto("string")
    }
}