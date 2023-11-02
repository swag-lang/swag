//#global skip
using Audio

#test
{
    assume Audio.createEngine()
    defer Audio.destroyEngine()

    assume
    {
        var x = SoundFile.load(sndPath("pacman_chomp.wav"), loadDatas: false)
        let voice = Voice.create(&x)
        voice.setVolume(0)
        voice.play()
    }

    Core.Threading.Thread.sleep(50'ms)
    voice.destroy()
}