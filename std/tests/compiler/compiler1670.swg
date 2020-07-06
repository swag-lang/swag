#module compiler1670
#unittest error

#run
{
    var x: [2] s32
    @print(@kindof(x[0]).name)
}