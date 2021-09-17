#global export

#if #os == "windows" and #swagos == "windows"
{
    #foreignlib "ucrt.lib"
    const ForeignLib = "ucrtbase"
}
#else
{
    #assert(false, "unsupported os")
}