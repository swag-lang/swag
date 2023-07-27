#global if #os == Swag.TargetOs.Windows
#test
{
    assume XAudio2.create()
    XAudio2.destroy()
}