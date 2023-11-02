//#global skip
using Audio

func test() throw
{
    Audio.createEngine()
    defer Audio.destroyEngine()
    //Audio.setOutputVolume(0.0)

    let bus = Bus.create(2)
    bus.setVolume(0.0)

    var x = SoundFile.load(sndPath("sngl08m.wav"), loadDatas: false)
    let voice = Voice.create(&x)
    voice.setRooting([bus])

    voice.play()
    Core.Threading.Thread.sleep(50'ms)
    voice.destroy()
}

#test
{
    assume test()
}