#global if #os == Swag.TargetOs.Windows
#test
{
    var drv: Audio.DriverXAudio2
    assume drv.create()
    drv.destroy()
}