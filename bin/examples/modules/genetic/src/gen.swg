using Core

const PopulationCount = 200'u32
const TakeBest        = 50'u32
const TakeWorst       = 0'u32

#[Swag.Tls]
var g_Rng:          Random.Rng
var g_NumGenes:     u32
var g_Gen:          [2] Generation
var g_CurGen:       u32
var g_TotalWeight   = 0'f32
var g_BestScore     = Swag.F32.Max
var g_NumGen:       u32
var g_Result:       Array'u8

struct Generation
{
    all: ArrayPtr'Solution
}

struct Solution
{
    genes:  Array'u32
    score:  f32
    weight: f32
}

func genInit()
{
    assume Jobs.setNumWorkers()

    g_NumGenes = g_Image0.width * g_Image0.height

    // Generate initial population
    loop PopulationCount
    {
        let soluce = g_Gen[0].all.addNewPtr()
        soluce.genes.resize(cast(u64) g_NumGenes)
        loop i: g_NumGenes soluce.genes[i] = i
        g_Rng.shuffle(soluce.genes.toSlice())
        evaluateSolution(soluce)
    }

    // Sort solutions from best to worst
    g_Gen[0].all.sort(func(x, y) {
        return x.score <=> y.score
    })

    // Placeholder for the next generation
    loop PopulationCount
    {
        let soluce = g_Gen[1].all.addNewPtr()
        soluce.genes.resize(cast(u64) g_NumGenes)
    }

    g_Result.resize(cast(u64) g_NumGenes * 3)
}

func evaluateSolution(idxSrc, idxDst: u32)->f32
{
    var a, b: Pixel.Color = undefined

    a.r = g_Image0.pixels[(idxSrc*3)+0]
    a.g = g_Image0.pixels[(idxSrc*3)+1]
    a.b = g_Image0.pixels[(idxSrc*3)+2]

    b.r = g_Image1.pixels[(idxDst*3)+0]
    b.g = g_Image1.pixels[(idxDst*3)+1]
    b.b = g_Image1.pixels[(idxDst*3)+2]

    let dist = a.getDistanceRgb(b)
    return dist
}

func evaluateSolution(soluce: *Solution)
{
    soluce.score = 0
    loop i: soluce.genes
        soluce.score += evaluateSolution(soluce.genes[i], acast i)
    soluce.score /= g_NumGenes
}

func pickToMate(other: *Solution = null)->*Solution
{
    var rnd = g_Rng.nextF32(0, g_TotalWeight)
    visit child: g_Gen[g_CurGen].all
    {
        if rnd <= child.weight and child != other
            return child
        rnd -= child.weight
    }

    return g_Gen[g_CurGen].all[0]
}

func mutate(child: *Solution, idx: u32)
{
    var mutRate, mutNum: u32

    switch
    {
    case idx < PopulationCount/2:
        mutRate = 2
        mutNum  = 1
    case idx < PopulationCount/4:
        mutRate = 50
        mutNum  = 1
    case idx < PopulationCount/2:
        mutRate = 50
        mutNum  = 5
    case idx < PopulationCount/2 + PopulationCount/4:
        mutRate = 50
        mutNum  = 10
    default:
        mutRate = 50
        mutNum  = 20
    }

    let rnd = g_Rng.nextU32(0, 100)
    if rnd <= mutRate
    {
        loop mutNum
        {
            let src = g_Rng.nextU32(0, g_NumGenes)
            let dst = g_Rng.nextU32(0, g_NumGenes)
            swap(child.genes.buffer + src, child.genes.buffer + dst)
        }
    }
}

/*func mutate(child: *Solution)
{
    var rnd = g_Rng.nextU32(0, 100)
    if rnd <= MutationRate
    {
        var rnd1 = g_Rng.nextU32(0, MaxShuffle)
        loop rnd1
        {
            var src = g_Rng.nextU32(0, g_NumGenes)
            var dst = g_Rng.nextU32(0, g_NumGenes)
            swap(child.genes.buffer + src, child.genes.buffer + dst)
        }
    }
}*/

func copyToNext(idxNext, idxCur: u32)
{
    let nextGenIdx = (g_CurGen + 1) % 2
    g_Gen[nextGenIdx].all[idxNext].genes = g_Gen[g_CurGen].all[idxCur].genes
    evaluateSolution(g_Gen[nextGenIdx].all[idxNext])
}

func genGo()->*u8
{
    g_NumGen += 1

    // Compute stuff for selection
    g_TotalWeight = 0'f32
    visit child: g_Gen[g_CurGen].all
    {
        child.weight = 1 - child.score
        g_TotalWeight += child.weight
    }

    let nextGenIdx = (g_CurGen + 1) % 2
    var numChilds = PopulationCount - TakeBest - TakeWorst

    Jobs.parallelFor(|index| numChilds)
    {
        var done: Array'bool
        done.resize(cast(u64) g_NumGenes)

        let nextGenIdx = (g_CurGen + 1) % 2

        // Selection
        let father = pickToMate()
        let mother = pickToMate(father)

        // Cross point
        let crossPoint1 = g_Rng.nextU32(1, g_NumGenes - 2)

        // Copy solution from father
        let child1 = g_Gen[nextGenIdx].all[index]
        Memory.copy(child1.genes.buffer, father.genes.buffer, cast(u64) crossPoint1 * @sizeof(u32))
        loop i: crossPoint1
            done[child1.genes.buffer[i]] = true

        // Copy solution from mother, in order, except of already present
        var holes: Array'u32
        var cpt = 0'u32
        loop i: g_NumGenes - crossPoint1
        {
            let g = mother.genes[i + crossPoint1]
            if !done[g]
            {
                done[g] = true
                child1.genes[crossPoint1 + cpt] = g
            }
            else
            {
                holes.add(crossPoint1 + cpt)
            }

            cpt += 1
        }

        // Fill the rest
        loop i: g_NumGenes
        {
            if done[i] continue

            var bestRes = Swag.F32.Max
            var bestHole = 0'u64

            for var idx = 0; idx < Math.min(50, holes.count); idx += 1
            {
                let start = g_Rng.nextU32(0, acast holes.count)
                let hole = holes[start]
                let result = evaluateSolution(i, hole)
                if result < bestRes
                {
                    bestRes = result
                    bestHole = start
                }
            }

            child1.genes[holes[bestHole]] = i
            holes.removeAt(bestHole)
        }

        evaluateSolution(child1)
    }

    // Get from previous generation
    loop i: TakeBest
    {
        copyToNext(numChilds, i)
        numChilds += 1
    }

    loop i: TakeWorst
    {
        copyToNext(numChilds, PopulationCount - i - 1)
        numChilds += 1
    }

    // Mutate
    g_Gen[nextGenIdx].all.sort(func(x, y) {
        return x.score <=> y.score
    })

    visit child, idx: g_Gen[nextGenIdx].all
    {
        mutate(child, acast idx)
        evaluateSolution(child)
    }

    g_CurGen = (g_CurGen + 1) % 2

    // Sort solutions from best to worst
    g_Gen[g_CurGen].all.sort(func(x, y) {
        return x.score <=> y.score
    })

    // Returns the best score
    let best = g_Gen[g_CurGen].all[0]
    let bestScore = best.score
    if bestScore < g_BestScore
    {
        g_BestScore = bestScore
        Console.print(g_NumGen, ": ", bestScore, "\n")

        loop i: best.genes
        {
            let idx = best.genes[i]
            g_Result[(i*3)+0] = g_Image0.pixels[(idx*3)+0]
            g_Result[(i*3)+1] = g_Image0.pixels[(idx*3)+1]
            g_Result[(i*3)+2] = g_Image0.pixels[(idx*3)+2]
        }

        return g_Result.buffer
    }

    return null
}