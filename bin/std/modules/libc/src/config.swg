#global export

#if #os == "windows" and #swagos == "windows"
{
    #foreignlib "ucrt"
    const ForeignLib = "ucrtbase"
}
#else
{
    #assert(false, "unsupported os")
}