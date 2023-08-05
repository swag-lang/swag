using Audio

func test() throw
{
    Audio.createEngine()
    defer Audio.destroyEngine()
    Audio.setOutputVolume(0.0)

    var x = SoundFile.load(sndPath("sngl08m.wav"), loadDatas = false)
    let voice = Voice.create(&x)
    voice.play()
    Core.Threading.Thread.sleep(500'ms)
    //while voice.isPlaying() {}
}

#test
{
    assume test()
}