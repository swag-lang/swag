#global skip
using Core, Audio

#test
{
    var files = assume Directory.enumerateFiles(dataPath(), matchExtension = ".wav")
    visit f: files
    {
        var _x = assume SoundFile.load(f.fullname)
    }
}