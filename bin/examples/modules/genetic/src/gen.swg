using Core

const PopulationCount = 200'u32
const TakeBest        = 20
const TakeWorst       = 4
const MaxShuffle      = 128'u32
const MutationRate    = 2

#[Swag.Tls]
var g_Rng:          Random.DefaultRng
var g_NumGenes:     u32
var g_Gen:          [2] Generation
var g_CurGen:       u32
var g_TotalWeight   = 0'f32
var g_BestScore     = Swag.F32.Max
var g_NumGen:       u32

struct Generation
{
    all: ArrayPtr'Solution
}

struct Solution
{
    genes:  Array'u8
    score:  f32
    weight: f32
}

func genInit()
{
    assume Jobs.setNumWorkers()

    g_NumGenes = g_Image0.width * g_Image0.height

    // Generate initial population
    var ranks: Array'u32
    loop PopulationCount
    {
        ranks.resize(cast(uint) g_NumGenes)
        loop i: g_NumGenes
            ranks[i] = i

        soluce := g_Gen[0].all.addNewPtr()
        soluce.genes.resize(cast(uint) g_NumGenes * 3)

        Memory.copy(soluce.genes.buffer, g_Image0.pixels, cast(uint) g_NumGenes * 3)

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
        soluce.genes.resize(cast(uint) g_NumGenes * 3)
    }
}

func evaluateSolution(soluce: *Solution)
{
    soluce.score = 0
    for idx := 0; idx < soluce.genes.count; idx += 3
    {
        var a, b: Pixel.Color = ?

        a.r = soluce.genes[idx]
        a.g = soluce.genes[idx+1]
        a.b = soluce.genes[idx+2]

        b.r = g_Image1.pixels[idx]
        b.g = g_Image1.pixels[idx+1]
        b.b = g_Image1.pixels[idx+2]

        dist := a.getDistanceRgb(b)
        soluce.score += dist
    }

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

func mutate(child: *Solution)
{
    rnd := g_Rng.nextU32(0, 100)
    if rnd <= MutationRate
    {
        rnd1 := g_Rng.nextU32(0, MaxShuffle)
        loop rnd1
        {
            src := g_Rng.nextU32(0, cast(u32) g_NumGenes) * 3
            dst := g_Rng.nextU32(0, cast(u32) g_NumGenes) * 3
            swap(child.genes.buffer + src, child.genes.buffer + dst)
            swap(child.genes.buffer + (src+1), child.genes.buffer + (dst+1))
            swap(child.genes.buffer + (src+2), child.genes.buffer + (dst+2))
        }
    }
}

func copyToNext(idxNext, idxCur: u32)
{
    nextGenIdx := (g_CurGen + 1) % 2
    Memory.copy(g_Gen[nextGenIdx].all[idxNext].genes.buffer, g_Gen[g_CurGen].all[idxCur].genes.buffer, cast(uint) g_NumGenes * 3)
    evaluateSolution(g_Gen[nextGenIdx].all[idxNext])
}

func genGo()->*Solution
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

        // Cross point
        crossPoint1 := cast(uint) g_Rng.nextU32(1, cast(u32) g_NumGenes - 2)
        crossPoint1 *= 3
        crossPoint2 := cast(uint) g_Rng.nextU32(1, cast(u32) g_NumGenes - 2)
        crossPoint2 *= 3
        if crossPoint2 < crossPoint1
            swap(&crossPoint1, &crossPoint2)

        // Cross chromosomes
        child1 := g_Gen[nextGenIdx].all[index]
        Memory.copy(child1.genes.buffer, father.genes.buffer, crossPoint1)
        Memory.copy(child1.genes.buffer + crossPoint1, mother.genes.buffer + crossPoint1, (crossPoint2 - crossPoint1))
        Memory.copy(child1.genes.buffer + crossPoint2, father.genes.buffer + crossPoint2, (father.genes.count - crossPoint2))
        mutate(child1)
        evaluateSolution(child1)
    }

    // Get from previous generation
    loop i: TakeBest
    {
        copyToNext(numChilds, cast(u32) i)
        numChilds += 1
    }

    loop i: TakeWorst
    {
        copyToNext(numChilds, PopulationCount - i - 1)
        numChilds += 1
    }

    g_CurGen = (g_CurGen + 1) % 2

    // Sort solutions from best to worst
    g_Gen[g_CurGen].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Returns the best score
    bestScore := g_Gen[g_CurGen].all[0].score
    if bestScore < g_BestScore
    {
        g_BestScore = bestScore
        Console.print(g_NumGen, ": ", bestScore, "\n")
    }

    return g_Gen[g_CurGen].all[0]
}