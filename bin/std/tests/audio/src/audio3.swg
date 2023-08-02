#global if #os == Swag.TargetOs.Windows
using Audio

#test
{
    var drv: DriverXAudio2
    assume drv.create()
    defer drv.destroy()

    var x: SoundWav
    assume x.read(sndPath("snd1.wav"))

    assume drv.createVoice(&x)
}