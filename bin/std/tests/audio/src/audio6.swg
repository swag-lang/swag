using Audio

func test() throw
{
    Audio.createEngine()
    defer Audio.destroyEngine()

    var x = SoundFile.load(sndPath("pacman_eatfruit.wav"), loadDatas: false)
    let voice = Voice.create(&x)
    voice.setVolume(0)
    voice.play(.Loop)
    Core.Threading.Thread.sleep(20'ms)
    voice.stop()
    Core.Threading.Thread.sleep(20'ms)
    voice.play(.Loop)
    Core.Threading.Thread.sleep(1's)
}

#test
{
    assume test()
}