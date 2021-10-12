#global export

#if #os == "windows"
{
    #foreignlib "ucrt"
    const ForeignLib = "ucrtbase"
}
#else
{
    #assert(false, "unsupported os")
}