#global if #os == Swag.TargetOs.Windows
using Audio

#test
{
    assume XAudio2.create()
    defer XAudio2.destroy()

    var x: SoundWav
    assume x.read(sndPath("snd1.wav"))
}