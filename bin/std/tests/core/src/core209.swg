using Core

var G   = 0's64
const N = 5

func entry(th: Threading.Thread)
{
    var x: String = "this is a very long string this is a very long string"
}

#test
{
    var th: [N] Threading.Thread

    visit &t: th do
        catch t.init(&entry)
    visit &t: th do
        catch t.start()
    loop i: N do
        Threading.wait(&th[i])
}
