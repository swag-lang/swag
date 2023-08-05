using Audio

func test() throw
{
    Audio.createEngine()
    defer Audio.destroyEngine()

    var x = SoundFile.load(sndPath("pcm3208m.wav"), loadDatas = false)
    let voice = Voice.create(&x)
    voice.play()
    while voice.isPlaying() {}
}

#test
{
    assume test()
}