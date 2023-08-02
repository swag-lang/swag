#global if #os == Swag.TargetOs.Windows
using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()

    //var drv: DriverXAudio2
    //assume drv.create()
    //defer drv.destroy()

    var x: SoundWav
    assume x.read(sndPath("snd1.wav"))

    var voice = assume Voice.create(&x)
    assume voice.play()
    assume voice.setVolume(0)

    while voice.isPlaying() {}
    voice.destroy()
}