#global export

#if #os == Swag.TargetOs.Windows
{
    #foreignlib("ucrt")
    const ForeignLib = "ucrtbase"
}
#else
{
    #error("unsupported os")
}
