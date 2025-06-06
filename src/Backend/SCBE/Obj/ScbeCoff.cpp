#include "pch.h"
#include "Backend/SCBE/Obj/ScbeCoff.h"
#include "Backend/SCBE/Debug/ScbeDebugCodeView.h"
#include "Backend/SCBE/Main/Scbe.h"
#include "Wmf/Module.h"
#ifdef SWAG_STATS
#include "Main/Statistics.h"
#endif

namespace
{
    constexpr int UWOP_PUSH_NO_VOL = 0x00000000;
    constexpr int UWOP_ALLOC_LARGE = 0x00000001;
    constexpr int UWOP_ALLOC_SMALL = 0x00000002;

    bool emitXData(const BuildParameters&, ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        concat.align(16);
        *pp.patchXDOffset = concat.totalCount();

        // https://docs.microsoft.com/en-us/cpp/build/exception-handling-x64?view=vs-2019
        uint32_t offset = 0;
        for (const auto f : pp.functions)
        {
            f->xdataOffset = offset;
            ScbeCoff::emitUnwind(pp.concat, offset, f->sizeProlog, f->unwind);
        }

        *pp.patchXDCount = concat.totalCount() - *pp.patchXDOffset;
        return true;
    }

    bool emitPData(const BuildParameters&, ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        concat.align(16);
        *pp.patchPDOffset = concat.totalCount();

        uint32_t offset = 0;
        for (const auto f : pp.functions)
        {
            SWAG_ASSERT(f->symbolIndex < pp.allSymbols.size());
            SWAG_ASSERT(f->endAddress > f->startAddress);

            CpuRelocation reloc;
            reloc.type = IMAGE_REL_AMD64_ADDR32NB;

            reloc.virtualAddress = offset;
            reloc.symbolIndex    = f->symbolIndex;
            pp.relocTablePDSection.table.push_back(reloc);
            concat.addU32(0);

            reloc.virtualAddress = offset + 4;
            reloc.symbolIndex    = f->symbolIndex;
            pp.relocTablePDSection.table.push_back(reloc);
            concat.addU32(f->endAddress - f->startAddress);

            reloc.virtualAddress = offset + 8;
            reloc.symbolIndex    = pp.symXDIndex;
            pp.relocTablePDSection.table.push_back(reloc);
            concat.addU32(f->xdataOffset);
            SWAG_ASSERT(f->xdataOffset < *pp.patchXDCount);

            offset += 12;
        }

        *pp.patchPDCount = concat.totalCount() - *pp.patchPDOffset;
        return true;
    }

    bool emitDirectives(const BuildParameters&, ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        if (pp.directives.empty())
            return true;
        *pp.patchDROffset = concat.totalCount();
        *pp.patchDRCount  = pp.directives.length();
        concat.addStringN(pp.directives.data(), pp.directives.length());
        return true;
    }

    bool emitSymbolTable(const BuildParameters&, ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        *pp.patchSymbolTableOffset = concat.totalCount();
        SWAG_ASSERT(pp.allSymbols.size() <= UINT32_MAX);
        *pp.patchSymbolTableCount = pp.allSymbols.size();

        pp.stringTableOffset = 4;
        for (auto& symbol : pp.allSymbols)
        {
            // .Name
            if (symbol.name.length() <= 8)
            {
                // Be sure it's stuffed with 0 after the name, or we can have weird things
                // in the compiler
                concat.addU64(0);
                const auto ptr = concat.getSeekPtr() - 8;
                std::copy_n(symbol.name.buffer, symbol.name.length(), ptr);
            }
            else
            {
                concat.addU32(0);
                concat.addU32(pp.stringTableOffset);
                pp.stringTable.push_back(&symbol.name);
                pp.stringTableOffset += symbol.name.length() + 1;
            }

            concat.addU32(symbol.value); // .Value
            switch (symbol.kind)
            {
                case CpuSymbolKind::Function:
                    concat.addU16(pp.sectionIndexText);           // .SectionNumber
                    concat.addU16(IMAGE_SYM_DTYPE_FUNCTION << 8); // .Type
                    concat.addU8(IMAGE_SYM_CLASS_EXTERNAL);       // .StorageClass
                    concat.addU8(0);                              // .NumberOfAuxSymbols
                    break;
                case CpuSymbolKind::Extern:
                    concat.addU16(0);                       // .SectionNumber
                    concat.addU16(0);                       // .Type
                    concat.addU8(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
                    concat.addU8(0);                        // .NumberOfAuxSymbols
                    break;
                case CpuSymbolKind::Custom:
                    concat.addU16(symbol.sectionIdx);       // .SectionNumber
                    concat.addU16(0);                       // .Type
                    concat.addU8(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
                    concat.addU8(0);                        // .NumberOfAuxSymbols
                    break;
                case CpuSymbolKind::Constant:
                    concat.addU16(pp.sectionIndexSS);     // .SectionNumber
                    concat.addU16(0);                     // .Type
                    concat.addU8(IMAGE_SYM_CLASS_STATIC); // .StorageClass
                    concat.addU8(0);                      // .NumberOfAuxSymbols
                    break;
                default:
                    SWAG_ASSERT(false);
                    break;
            }
        }

        return true;
    }

    bool emitStringTable(const BuildParameters&, ScbeCpu& pp)
    {
        auto& concat = pp.concat;

        concat.addU32(pp.stringTableOffset); // .Size of table in bytes + 4
        SWAG_IF_ASSERT(uint32_t subTotal = 4);
        for (const auto str : pp.stringTable)
        {
            concat.addStringN(str->buffer, str->count);
            concat.addU8(0);
            SWAG_IF_ASSERT(subTotal += str->count + 1);
        }

        SWAG_ASSERT(subTotal == pp.stringTableOffset);
        return true;
    }
}

bool ScbeCoff::emitHeader(const BuildParameters& buildParameters, ScbeCpu& pp)
{
    const auto           precompileIndex = buildParameters.precompileIndex;
    const auto           module          = buildParameters.module;
    auto&                concat          = pp.concat;
    static constexpr int NUM_SECTIONS_0  = 12;
    static constexpr int NUM_SECTIONS_X  = 7;
    // Coff header
    /////////////////////////////////////////////
    concat.init();
    concat.addU16(IMAGE_FILE_MACHINE_AMD64); // .Machine
    if (precompileIndex == 0)
        concat.addU16(NUM_SECTIONS_0); // .NumberOfSections
    else
        concat.addU16(NUM_SECTIONS_X); // .NumberOfSections

    time_t now;
    (void) time(&now);
    // concat.addU32((uint32_t)(now & 0xFFFFFFFF)); // .TimeDateStamp
    concat.addU32(0);

    pp.patchSymbolTableOffset = concat.addU32Addr(0); // .PointerToSymbolTable
    pp.patchSymbolTableCount  = concat.addU32Addr(0); // .NumberOfSymbols

    concat.addU16(0); // .SizeOfOptionalHeader

    concat.addU16(IMAGE_FILE_LARGE_ADDRESS_AWARE | IMAGE_FILE_DEBUG_STRIPPED); // .Characteristics

    // Code section
    /////////////////////////////////////////////
    uint16_t secIndex   = 1;
    pp.sectionIndexText = secIndex++;
    concat.addStringN(".text\0\0\0", 8);                        // .Name
    concat.addU32(0);                                           // .VirtualSize
    concat.addU32(0);                                           // .VirtualAddress
    pp.patchTextSectionSize             = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchTextSectionOffset           = concat.addU32Addr(0); // .PointerToRawData
    pp.patchTextSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLineNumbers
    pp.patchTextSectionRelocTableCount = concat.addU16Addr(0);  // .PointerToRelocations
    concat.addU16(0);                                           // .NumberOfLineNumbers
    pp.patchTextSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_16BYTES);

    // generated constants sections
    /////////////////////////////////////////////
    pp.sectionIndexSS = secIndex++;
    concat.addStringN(".rdata\0\0", 8);      // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchSSCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchSSOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLineNumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLineNumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES);

    // directive section (to register dll exported symbols)
    pp.sectionIndexDR = secIndex++;
    concat.addStringN(".drectve", 8);        // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchDRCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDROffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLineNumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLineNumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES | IMAGE_SCN_LNK_INFO);

    // .pdata section (to register functions)
    pp.sectionIndexPD = secIndex++;
    concat.addStringN(".pdata\0\0", 8);                       // .Name
    concat.addU32(0);                                         // .VirtualSize
    concat.addU32(0);                                         // .VirtualAddress
    pp.patchPDCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchPDOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchPDSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                         // .PointerToLineNumbers
    pp.patchPDSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                         // .NumberOfLineNumbers
    pp.patchPDSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .xdata section (for unwind infos)
    pp.sectionIndexXD = secIndex++;
    concat.addStringN(".xdata\0\0", 8);      // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchXDCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchXDOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLineNumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLineNumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .debug$S section
    pp.sectionIndexDBGS = secIndex++;
    concat.addStringN(".debug$S", 8);                           // .Name
    concat.addU32(0);                                           // .VirtualSize
    concat.addU32(0);                                           // .VirtualAddress
    pp.patchDBGSCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDBGSOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchDBGSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLineNumbers
    pp.patchDBGSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                           // .NumberOfLineNumbers
    pp.patchDBGSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .debug$T section
    pp.sectionIndexDBGT = secIndex++;
    concat.addStringN(".debug$T", 8);          // .Name
    concat.addU32(0);                          // .VirtualSize
    concat.addU32(0);                          // .VirtualAddress
    pp.patchDBGTCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDBGTOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                          // .PointerToRelocations
    concat.addU32(0);                          // .PointerToLineNumbers
    concat.addU16(0);                          // .NumberOfRelocations
    concat.addU16(0);                          // .NumberOfLineNumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    if (precompileIndex == 0)
    {
        // constant section
        // This is not readonly because we can patch some stuff during the initialization stage of the module
        /////////////////////////////////////////////
        pp.sectionIndexCS = secIndex++;
        concat.addStringN(".data\0\0\0", 8);                      // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchCSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchCSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchCSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLineNumbers
        pp.patchCSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLineNumbers
        pp.patchCSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // bss section
        /////////////////////////////////////////////
        pp.sectionIndexBS = secIndex++;
        concat.addStringN(".bss\0\0\0\0", 8);         // .Name
        concat.addU32(0);                             // .VirtualSize
        concat.addU32(0);                             // .VirtualAddress
        concat.addU32(module->bssSegment.totalCount); // .SizeOfRawData
        concat.addU32(0);                             // .PointerToRawData
        concat.addU32(0);                             // .PointerToRelocations
        concat.addU32(0);                             // .PointerToLineNumbers
        concat.addU16(0);                             // .NumberOfRelocations
        concat.addU16(0);                             // .NumberOfLineNumbers
        concat.addU32(IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // global section
        /////////////////////////////////////////////
        pp.sectionIndexGS = secIndex++;
        concat.addStringN(".data\0\0\0", 8);     // .Name
        concat.addU32(0);                        // .VirtualSize
        concat.addU32(0);                        // .VirtualAddress
        pp.patchGSCount  = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchGSOffset = concat.addU32Addr(0); // .PointerToRawData
        concat.addU32(0);                        // .PointerToRelocations
        concat.addU32(0);                        // .PointerToLineNumbers
        concat.addU16(0);                        // .NumberOfRelocations
        concat.addU16(0);                        // .NumberOfLineNumbers
        (void) concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // mutable section
        /////////////////////////////////////////////
        pp.sectionIndexMS = secIndex++;
        concat.addStringN(".data\0\0\0", 8);                      // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchMSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchMSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchMSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLineNumbers
        pp.patchMSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLineNumbers
        pp.patchMSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // tls section
        /////////////////////////////////////////////
        // ReSharper disable once CppAssignedValueIsNeverUsed
        pp.sectionIndexTLS = secIndex++;
        concat.addStringN(".data\0\0\0", 8);                       // .Name
        concat.addU32(0);                                          // .VirtualSize
        concat.addU32(0);                                          // .VirtualAddress
        pp.patchTLSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchTLSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchTLSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                          // .PointerToLineNumbers
        pp.patchTLSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                          // .NumberOfLineNumbers
        pp.patchTLSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);
    }

    SWAG_ASSERT(precompileIndex != 0 || secIndex == NUM_SECTIONS_0 + 1);
    SWAG_ASSERT(precompileIndex == 0 || secIndex == NUM_SECTIONS_X + 1);

    return true;
}

void ScbeCoff::emitUnwind(Concat& concat, uint32_t& offset, uint32_t sizeProlog, const VectorNative<uint16_t>& unwind)
{
    SWAG_ASSERT(sizeProlog <= 255);

    concat.addU8(1);                                   // Version
    concat.addU8(static_cast<uint8_t>(sizeProlog));    // Size of prolog
    concat.addU8(static_cast<uint8_t>(unwind.size())); // Count of unwind codes
    concat.addU8(0);                                   // Frame register | offset
    offset += 4;

    // Unwind array
    for (const auto un : unwind)
    {
        concat.addU16(un);
        offset += 2;
    }

    // Align
    if (unwind.size() & 1)
    {
        concat.addU16(0);
        offset += 2;
    }
}

bool ScbeCoff::emitRelocationTable(Concat& concat, const CpuRelocationTable& coffTable, uint32_t* sectionFlags, uint16_t* count)
{
    SWAG_ASSERT(coffTable.table.size() < UINT32_MAX);
    const auto tableSize = coffTable.table.size();
    if (tableSize > 0xFFFF)
    {
        *count = 0xFFFF;
        *sectionFlags |= IMAGE_SCN_LNK_NRELOC_OVFL;
        concat.ensureSpace(4 + 4 + 2);
        concat.addU32(tableSize + 1);
        concat.addU32(0);
        concat.addU16(0);
    }
    else
    {
        *count = static_cast<uint16_t>(tableSize);
    }

    for (const auto& reloc : coffTable.table)
    {
        concat.ensureSpace(4 + 4 + 2);
        concat.addU32(reloc.virtualAddress);
        concat.addU32(reloc.symbolIndex);
        concat.addU16(reloc.type);
    }

    return true;
}

bool ScbeCoff::emitPostFunc(const BuildParameters& buildParameters, ScbeCpu& pp)
{
    const auto module          = buildParameters.module;
    const auto precompileIndex = buildParameters.precompileIndex;
    auto&      concat          = pp.concat;

    emitSymbolTable(buildParameters, pp);
    emitStringTable(buildParameters, pp);
    emitDirectives(buildParameters, pp);
    emitXData(buildParameters, pp);
    emitPData(buildParameters, pp);

#ifdef SWAG_STATS
    const auto beforeCount = pp.concat.totalCount();
#endif
    ScbeDebugCodeView::emit(buildParameters, pp);
#ifdef SWAG_STATS
    g_Stats.sizeScbeDbg += pp.concat.totalCount() - beforeCount;
#endif

    if (!pp.relocTableTextSection.table.empty())
    {
        concat.align(16);
        *pp.patchTextSectionRelocTableOffset = concat.totalCount();
        emitRelocationTable(pp.concat, pp.relocTableTextSection, pp.patchTextSectionFlags, pp.patchTextSectionRelocTableCount);
    }

    if (!pp.relocTablePDSection.table.empty())
    {
        concat.align(16);
        *pp.patchPDSectionRelocTableOffset = concat.totalCount();
        emitRelocationTable(pp.concat, pp.relocTablePDSection, pp.patchPDSectionFlags, pp.patchPDSectionRelocTableCount);
    }

    // Segments
    const uint32_t ssOffset = concat.totalCount();
    if (pp.constantSegment.totalCount)
    {
        *pp.patchSSCount  = pp.constantSegment.totalCount;
        *pp.patchSSOffset = ssOffset;
    }

    if (precompileIndex == 0)
    {
        const uint32_t gsOffset  = ssOffset + pp.constantSegment.totalCount;
        const uint32_t csOffset  = gsOffset + pp.globalSegment.totalCount;
        const uint32_t msOffset  = csOffset + module->constantSegment.totalCount;
        const uint32_t tlsOffset = msOffset + module->mutableSegment.totalCount;

        // We do not use concat to avoid dummy copies. We will save the segments as they are
        if (pp.globalSegment.totalCount)
        {
            *pp.patchGSCount  = pp.globalSegment.totalCount;
            *pp.patchGSOffset = gsOffset;
        }

        if (module->constantSegment.totalCount)
        {
            *pp.patchCSCount  = module->constantSegment.totalCount;
            *pp.patchCSOffset = csOffset;
        }

        if (module->mutableSegment.totalCount)
        {
            *pp.patchMSCount  = module->mutableSegment.totalCount;
            *pp.patchMSOffset = msOffset;
        }

        if (module->tlsSegment.totalCount)
        {
            *pp.patchTLSCount  = module->tlsSegment.totalCount;
            *pp.patchTLSOffset = tlsOffset;
        }

        // And we use another concat buffer for relocation tables of segments, because they must be defined after
        // the content
        pp.postConcat.init();

        // Warning! Should be the last segment
        const uint32_t csRelocOffset = tlsOffset + module->tlsSegment.totalCount;

        if (!pp.relocTableCSSection.table.empty())
        {
            *pp.patchCSSectionRelocTableOffset = csRelocOffset;
            emitRelocationTable(pp.postConcat, pp.relocTableCSSection, pp.patchCSSectionFlags, pp.patchCSSectionRelocTableCount);
        }

        const uint32_t msRelocOffset = csRelocOffset + pp.postConcat.totalCount();
        if (!pp.relocTableMSSection.table.empty())
        {
            *pp.patchMSSectionRelocTableOffset = msRelocOffset;
            emitRelocationTable(pp.postConcat, pp.relocTableMSSection, pp.patchMSSectionFlags, pp.patchMSSectionRelocTableCount);
        }

        const uint32_t tlsRelocOffset = csRelocOffset + pp.postConcat.totalCount();
        if (!pp.relocTableTLSSection.table.empty())
        {
            *pp.patchTLSSectionRelocTableOffset = tlsRelocOffset;
            emitRelocationTable(pp.postConcat, pp.relocTableTLSSection, pp.patchTLSSectionFlags, pp.patchTLSSectionRelocTableCount);
        }
    }

    return true;
}

bool ScbeCoff::saveFileBuffer(FILE* f, const BuildParameters& buildParameters, ScbeCpu& pp)
{
    const auto module          = buildParameters.module;
    const auto precompileIndex = buildParameters.precompileIndex;

    // Output the full concat buffer
    SWAG_IF_ASSERT(uint32_t totalCount = 0);
    auto bucket = pp.concat.firstBucket;
    while (bucket != pp.concat.lastBucket->nextBucket)
    {
        const auto count = pp.concat.bucketCount(bucket);
        (void) fwrite(bucket->data, count, 1, f);
        SWAG_IF_ASSERT(totalCount += count);
        bucket = bucket->nextBucket;
    }

    SWAG_IF_ASSERT(uint32_t subTotal = 0);
    SWAG_ASSERT(totalCount == pp.concat.totalCount());

    // The global strings segment
    SWAG_ASSERT(totalCount == *pp.patchSSOffset || *pp.patchSSOffset == 0);
    for (const auto oneB : pp.constantSegment.buckets)
    {
        SWAG_IF_ASSERT(totalCount += oneB.count);
        SWAG_IF_ASSERT(subTotal += oneB.count);
        (void) fwrite(oneB.buffer, oneB.count, 1, f);
    }
    SWAG_ASSERT(subTotal == pp.constantSegment.totalCount);

    if (precompileIndex == 0)
    {
        // The global segment to store context & process infos
        SWAG_ASSERT(totalCount == *pp.patchGSOffset || *pp.patchGSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (const auto oneB : pp.globalSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            (void) fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchGSCount || *pp.patchGSCount == 0);
        SWAG_ASSERT(subTotal == pp.globalSegment.totalCount);

        // The constant segment
        SWAG_ASSERT(totalCount == *pp.patchCSOffset || *pp.patchCSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (const auto oneB : module->constantSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            (void) fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchCSCount || *pp.patchCSCount == 0);
        SWAG_ASSERT(subTotal == module->constantSegment.totalCount);

        // The mutable segment
        SWAG_ASSERT(totalCount == *pp.patchMSOffset || *pp.patchMSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (const auto oneB : module->mutableSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            (void) fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchMSCount || *pp.patchMSCount == 0);
        SWAG_ASSERT(subTotal == module->mutableSegment.totalCount);

        // The tls segment
        SWAG_ASSERT(totalCount == *pp.patchTLSOffset || *pp.patchTLSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (const auto oneB : module->tlsSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            (void) fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchTLSCount || *pp.patchTLSCount == 0);
        SWAG_ASSERT(subTotal == module->tlsSegment.totalCount);

        // The post concat buffer that contains relocation tables for CS and DS
        bucket = pp.postConcat.firstBucket;
        while (bucket != pp.postConcat.lastBucket->nextBucket)
        {
            const auto count = pp.postConcat.bucketCount(bucket);
            (void) fwrite(bucket->data, count, 1, f);
            bucket = bucket->nextBucket;
        }
    }

    return true;
}

void ScbeCoff::computeUnwind(const VectorNative<CpuReg>& unwindRegs, const VectorNative<uint32_t>& unwindOffsetRegs, uint32_t sizeStack, uint32_t offsetSubRSP, VectorNative<uint16_t>& unwind)
{
    // UNWIND_CODE
    // UBYTE:8: offset of the instruction after the "sub rsp"
    // UBYTE:4: code (UWOP_ALLOC_LARGE or UWOP_ALLOC_SMALL)
    // UBYTE:4: info (will code the size of the decrement of rsp)

    SWAG_ASSERT(offsetSubRSP <= 0xFF);
    SWAG_ASSERT((sizeStack & 7) == 0); // Must be aligned

    if (sizeStack <= 128)
    {
        SWAG_ASSERT(sizeStack >= 8);
        sizeStack -= 8;
        sizeStack /= 8;
        auto unwind0 = static_cast<uint16_t>(UWOP_ALLOC_SMALL | sizeStack << 4);
        unwind0 <<= 8;
        unwind0 |= static_cast<uint16_t>(offsetSubRSP);
        unwind.push_back(unwind0);
    }
    else
    {
        SWAG_ASSERT(sizeStack <= 512 * 1024 - 8);
        auto unwind0 = static_cast<uint16_t>(UWOP_ALLOC_LARGE);
        unwind0 <<= 8;
        unwind0 |= static_cast<uint16_t>(offsetSubRSP);
        unwind.push_back(unwind0);
        unwind0 = static_cast<uint16_t>(sizeStack / 8);
        unwind.push_back(unwind0);
    }

    // Now we put the registers.
    // At the end because array must be sorted in 'offset in prolog' descending order.
    // So the first 'push' must be the last.
    for (uint32_t i = unwindRegs.size() - 1; i != UINT32_MAX; i--)
    {
        auto unwind0 = static_cast<uint16_t>(unwindRegs[i]) << 12;
        unwind0 |= UWOP_PUSH_NO_VOL << 8;
        unwind0 |= static_cast<uint8_t>(unwindOffsetRegs[i]);
        unwind.push_back(static_cast<uint16_t>(unwind0));
    }
}
