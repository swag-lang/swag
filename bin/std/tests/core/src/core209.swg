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

    foreach &t in th:
        catch t.init(&entry)
    foreach &t in th:
        catch t.start()
    for i in N:
        Threading.wait(&th[i])
}
