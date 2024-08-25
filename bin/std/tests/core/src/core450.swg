using Core

#test
{
    let str = assume File.readAllText(filePath("h1v1.jpg"))
    const N = 100

    {
        //Time.Stopwatch.scopeMeasure()
        for N:
            discard Hash.City32.compute(str.toSlice())
    }

    {
        //Time.Stopwatch.scopeMeasure()
        for N:
            discard Hash.Crc32.compute(str.toSlice())
    }

    //@assert(v == 0x86da05bf)
    //Console.printf("%{X}", v)
}
