#module compiler1298
#unittest error

func toto()
{
    x := 0
    #inline
    {
        x := 0
    }
}
