#global if #os == Swag.TargetOs.Windows
using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()

    var x: SoundWav

    assume
    {
        x.read(sndPath("snd1.wav"))
        var voice = Voice.create(&x)
        voice.setVolume(0)
        voice.play()
    }

    while voice.isPlaying() {}
    voice.destroy()
}