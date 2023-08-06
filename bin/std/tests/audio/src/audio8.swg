#global skip
using Audio

func test() throw
{
    Audio.createEngine()
    defer Audio.destroyEngine()
    //Audio.setOutputVolume(0.0)

    var bus = Bus.create(2)
    defer bus.destroy()

    var x = SoundFile.load(sndPath("sngl08m.wav"), loadDatas = false)
    let voice = Voice.create(&x)
    voice.setRooting([bus])

    voice.play()
    Core.Threading.Thread.sleep(500'ms)
    //while voice.isPlaying() {}
}

#test
{
    assume test()
}