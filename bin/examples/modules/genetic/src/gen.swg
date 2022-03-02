using Core

const PopulationCount = 200'u32
const TakeBest        = 50'u32
const TakeWorst       = 1'u32

#[Swag.Tls]
var g_Rng:          Random.DefaultRng
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
        soluce := g_Gen[0].all.addNewPtr()
        soluce.genes.resize(cast(uint) g_NumGenes)
        loop i: g_NumGenes
        {
            soluce.genes[i] = i
        }

        loop i: g_NumGenes
        {
            src := g_Rng.nextU32(0, g_NumGenes)
            dst := g_Rng.nextU32(0, g_NumGenes)
            swap(soluce.genes.buffer + src, soluce.genes.buffer + dst)
        }

        evaluateSolution(soluce)
    }

    // Sort solutions from best to worst
    g_Gen[0].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Placeholder for the next generation
    loop PopulationCount
    {
        soluce := g_Gen[1].all.addNewPtr()
        soluce.genes.resize(cast(uint) g_NumGenes)
    }

    g_Result.resize(cast(uint) g_NumGenes * 3)
}

func evaluateSolution(idxSrc, idxDst: u32)->f32
{
    var a, b: Pixel.Color = ?

    a.r = g_Image0.pixels[(idxSrc*3)+0]
    a.g = g_Image0.pixels[(idxSrc*3)+1]
    a.b = g_Image0.pixels[(idxSrc*3)+2]

    b.r = g_Image1.pixels[(idxDst*3)+0]
    b.g = g_Image1.pixels[(idxDst*3)+1]
    b.b = g_Image1.pixels[(idxDst*3)+2]

    x1 := cast(f32) a.r - b.r
    x2 := cast(f32) a.g - b.g
    x3 := cast(f32) a.b - b.b

    dist := Math.sqrt((x1*x1) + (x2*x2) + (x3*x3))
    dist /= 255*3
    //dist := a.getDistanceRgb(b)

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
    rnd := g_Rng.nextF32(0, g_TotalWeight)
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
    case idx < PopulationCount/4:
        mutRate = 5
        mutNum  = 10
    case idx < PopulationCount/2:
        mutRate = 25
        mutNum  = 100
    case idx < PopulationCount/2 + PopulationCount/4:
        mutRate = 50
        mutNum  = 500
    default:
        mutRate = 50
        mutNum  = 1000
    }

    rnd := g_Rng.nextU32(0, 100)
    if rnd <= mutRate
    {
        loop mutNum
        {
            src := g_Rng.nextU32(0, g_NumGenes)
            dst := g_Rng.nextU32(0, g_NumGenes)
            swap(child.genes.buffer + src, child.genes.buffer + dst)
        }
    }
}

func copyToNext(idxNext, idxCur: u32)
{
    nextGenIdx := (g_CurGen + 1) % 2
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

    nextGenIdx := (g_CurGen + 1) % 2
    numChilds := PopulationCount - TakeBest - TakeWorst

    Jobs.parallelFor(|index| numChilds)
    {
        nextGenIdx := (g_CurGen + 1) % 2

        // Selection
        father := pickToMate()
        mother := pickToMate(father)

        var done:   Array'bool
        var remain: Array'u32
        done.resize(cast(uint) g_NumGenes)

        // Copy solution from father
        child1 := g_Gen[nextGenIdx].all[index]
        for i := 0; i < g_NumGenes; i += 1
        {
            idx1 := father.genes[i]
            idx2 := mother.genes[i]

            var idxRes: u32
            var idxRemain: u32 = Swag.U32.Max

            if done[idx1] and done[idx2]
            {
                a := 0
                rank := 0'uint
                idxRes = remain[rank]
                remain.removeAt(rank)
            }
            elif !done[idx1] and done[idx2]
            {
                idxRes = idx1
            }
            elif done[idx1] and !done[idx2]
            {
                idxRes = idx2
            }
            else
            {
                dist1 := evaluateSolution(idx1, cast(u32) i)
                dist2 := evaluateSolution(idx2, cast(u32) i)
                /*if Math.abs(dist1 - dist2) < 0.1
                {
                    if g_Rng.nextU32(0, 2) == 1
                    {
                        idxRes = idx1
                        idxRemain = idx2
                    }
                    else
                    {
                        idxRes = idx2
                        idxRemain = idx1
                    }
                }
                el*/if dist1 < dist2
                {
                    idxRes = idx1
                    idxRemain = idx2
                }
                else
                {
                    idxRes = idx2
                    idxRemain = idx1
                }
            }

            child1.genes[i] = idxRes
            done[idxRes]    = true
            if idxRemain != Swag.U32.Max
                remain.add(idxRemain)
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
    g_Gen[nextGenIdx].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    visit c, idx: g_Gen[nextGenIdx].all
    {
        mutate(c, acast idx)
        evaluateSolution(c)
    }

    g_CurGen = (g_CurGen + 1) % 2

    // Sort solutions from best to worst
    g_Gen[g_CurGen].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Returns the best score
    best      := g_Gen[g_CurGen].all[0]
    bestScore := best.score
    if bestScore < g_BestScore
    {
        g_BestScore = bestScore
        Console.print(g_NumGen, ": ", bestScore, "\n")

        loop i: best.genes
        {
            idx := best.genes[i]
            g_Result[(i*3)+0] = g_Image0.pixels[(idx*3)+0]
            g_Result[(i*3)+1] = g_Image0.pixels[(idx*3)+1]
            g_Result[(i*3)+2] = g_Image0.pixels[(idx*3)+2]
        }

        return g_Result.buffer
    }

    return null
}