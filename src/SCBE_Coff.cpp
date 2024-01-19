#include "pch.h"
#include "SCBE.h"
#include "BackendLinker.h"
#include "ErrorIds.h"
#include "Module.h"
#include "Os.h"
#include "Report.h"
#include "SCBE_SaveObjJob.h"
#include "SCBE_Coff.h"
#include "Workspace.h"

bool SCBE_Coff::emitHeader(const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    int       precompileIndex = buildParameters.precompileIndex;
    auto      module          = buildParameters.module;
    auto&     concat          = pp.concat;
    const int NUM_SECTIONS_0  = 12;
    const int NUM_SECTIONS_X  = 7;

    // Coff header
    /////////////////////////////////////////////
    concat.init();
    concat.addU16(IMAGE_FILE_MACHINE_AMD64); // .Machine
    if (precompileIndex == 0)
        concat.addU16(NUM_SECTIONS_0); // .NumberOfSections
    else
        concat.addU16(NUM_SECTIONS_X); // .NumberOfSections

    time_t now;
    time(&now);
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
    concat.addString(".text\0\0\0", 8);                         // .Name
    concat.addU32(0);                                           // .VirtualSize
    concat.addU32(0);                                           // .VirtualAddress
    pp.patchTextSectionSize             = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchTextSectionOffset           = concat.addU32Addr(0); // .PointerToRawData
    pp.patchTextSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLinenumbers
    pp.patchTextSectionRelocTableCount = concat.addU16Addr(0);  // .PointerToRelocations
    concat.addU16(0);                                           // .NumberOfLinenumbers
    pp.patchTextSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_16BYTES);

    // global strings sections
    /////////////////////////////////////////////
    pp.sectionIndexSS = secIndex++;
    concat.addString(".rdata\0\0", 8);       // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchSSCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchSSOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLinenumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES);

    // directive section (to register dll exported symbols)
    pp.sectionIndexDR = secIndex++;
    concat.addString(".drectve", 8);         // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchDRCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDROffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLinenumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_1BYTES | IMAGE_SCN_LNK_INFO);

    // .pdata section (to register functions)
    pp.sectionIndexPD = secIndex++;
    concat.addString(".pdata\0\0", 8);                        // .Name
    concat.addU32(0);                                         // .VirtualSize
    concat.addU32(0);                                         // .VirtualAddress
    pp.patchPDCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchPDOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchPDSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                         // .PointerToLinenumbers
    pp.patchPDSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                         // .NumberOfLinenumbers
    pp.patchPDSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .xdata section (for unwind infos)
    pp.sectionIndexXD = secIndex++;
    concat.addString(".xdata\0\0", 8);       // .Name
    concat.addU32(0);                        // .VirtualSize
    concat.addU32(0);                        // .VirtualAddress
    pp.patchXDCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchXDOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                        // .PointerToRelocations
    concat.addU32(0);                        // .PointerToLinenumbers
    concat.addU16(0);                        // .NumberOfRelocations
    concat.addU16(0);                        // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .debug$S section
    pp.sectionIndexDBGS = secIndex++;
    concat.addString(".debug$S", 8);                            // .Name
    concat.addU32(0);                                           // .VirtualSize
    concat.addU32(0);                                           // .VirtualAddress
    pp.patchDBGSCount                   = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDBGSOffset                  = concat.addU32Addr(0); // .PointerToRawData
    pp.patchDBGSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
    concat.addU32(0);                                           // .PointerToLinenumbers
    pp.patchDBGSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
    concat.addU16(0);                                           // .NumberOfLinenumbers
    pp.patchDBGSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    // .debug$T section
    pp.sectionIndexDBGT = secIndex++;
    concat.addString(".debug$T", 8);           // .Name
    concat.addU32(0);                          // .VirtualSize
    concat.addU32(0);                          // .VirtualAddress
    pp.patchDBGTCount  = concat.addU32Addr(0); // .SizeOfRawData
    pp.patchDBGTOffset = concat.addU32Addr(0); // .PointerToRawData
    concat.addU32(0);                          // .PointerToRelocations
    concat.addU32(0);                          // .PointerToLinenumbers
    concat.addU16(0);                          // .NumberOfRelocations
    concat.addU16(0);                          // .NumberOfLinenumbers
    concat.addU32(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ | IMAGE_SCN_ALIGN_4BYTES);

    if (precompileIndex == 0)
    {
        // constant section
        // This is not readonly because we can patch some stuff during the initialization stage of the module
        /////////////////////////////////////////////
        pp.sectionIndexCS = secIndex++;
        concat.addString(".data\0\0\0", 8);                       // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchCSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchCSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchCSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchCSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchCSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // bss section
        /////////////////////////////////////////////
        pp.sectionIndexBS = secIndex++;
        concat.addString(".bss\0\0\0\0", 8);          // .Name
        concat.addU32(0);                             // .VirtualSize
        concat.addU32(0);                             // .VirtualAddress
        concat.addU32(module->bssSegment.totalCount); // .SizeOfRawData
        concat.addU32(0);                             // .PointerToRawData
        concat.addU32(0);                             // .PointerToRelocations
        concat.addU32(0);                             // .PointerToLinenumbers
        concat.addU16(0);                             // .NumberOfRelocations
        concat.addU16(0);                             // .NumberOfLinenumbers
        concat.addU32(IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // global section
        /////////////////////////////////////////////
        pp.sectionIndexGS = secIndex++;
        concat.addString(".data\0\0\0", 8);      // .Name
        concat.addU32(0);                        // .VirtualSize
        concat.addU32(0);                        // .VirtualAddress
        pp.patchGSCount  = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchGSOffset = concat.addU32Addr(0); // .PointerToRawData
        concat.addU32(0);                        // .PointerToRelocations
        concat.addU32(0);                        // .PointerToLinenumbers
        concat.addU16(0);                        // .NumberOfRelocations
        concat.addU16(0);                        // .NumberOfLinenumbers
        concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // mutable section
        /////////////////////////////////////////////
        pp.sectionIndexMS = secIndex++;
        concat.addString(".data\0\0\0", 8);                       // .Name
        concat.addU32(0);                                         // .VirtualSize
        concat.addU32(0);                                         // .VirtualAddress
        pp.patchMSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchMSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchMSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                         // .PointerToLinenumbers
        pp.patchMSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                         // .NumberOfLinenumbers
        pp.patchMSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);

        // tls section
        /////////////////////////////////////////////
        pp.sectionIndexTLS = secIndex++;
        concat.addString(".data\0\0\0", 8);                        // .Name
        concat.addU32(0);                                          // .VirtualSize
        concat.addU32(0);                                          // .VirtualAddress
        pp.patchTLSCount                   = concat.addU32Addr(0); // .SizeOfRawData
        pp.patchTLSOffset                  = concat.addU32Addr(0); // .PointerToRawData
        pp.patchTLSSectionRelocTableOffset = concat.addU32Addr(0); // .PointerToRelocations
        concat.addU32(0);                                          // .PointerToLinenumbers
        pp.patchTLSSectionRelocTableCount = concat.addU16Addr(0);  // .NumberOfRelocations
        concat.addU16(0);                                          // .NumberOfLinenumbers
        pp.patchTLSSectionFlags = concat.addU32Addr(IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_ALIGN_1BYTES);
    }

    SWAG_ASSERT(precompileIndex != 0 || secIndex == NUM_SECTIONS_0 + 1);
    SWAG_ASSERT(precompileIndex == 0 || secIndex == NUM_SECTIONS_X + 1);

    return true;
}

bool SCBE_Coff::emitXData(const BuildParameters& buildParameters, SCBE_X64& pp)
{
    auto& concat = pp.concat;

    concat.align(16);
    *pp.patchXDOffset = concat.totalCount();

    // https://docs.microsoft.com/en-us/cpp/build/exception-handling-x64?view=vs-2019
    uint32_t offset = 0;
    for (auto& f : pp.functions)
    {
        f.xdataOffset = offset;
        pp.emitUnwind(offset, f.sizeProlog, f.unwind);
    }

    *pp.patchXDCount = concat.totalCount() - *pp.patchXDOffset;
    return true;
}

bool SCBE_Coff::emitPData(const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    concat.align(16);
    *pp.patchPDOffset = concat.totalCount();

    uint32_t offset = 0;
    for (auto& f : pp.functions)
    {
        SWAG_ASSERT(f.symbolIndex < pp.allSymbols.size());
        SWAG_ASSERT(f.endAddress > f.startAddress);

        CPURelocation reloc;
        reloc.type = IMAGE_REL_AMD64_ADDR32NB;

        reloc.virtualAddress = offset;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTablePDSection.table.push_back(reloc);
        concat.addU32(0);

        reloc.virtualAddress = offset + 4;
        reloc.symbolIndex    = f.symbolIndex;
        pp.relocTablePDSection.table.push_back(reloc);
        concat.addU32(f.endAddress - f.startAddress);

        reloc.virtualAddress = offset + 8;
        reloc.symbolIndex    = pp.symXDIndex;
        pp.relocTablePDSection.table.push_back(reloc);
        concat.addU32(f.xdataOffset);
        SWAG_ASSERT(f.xdataOffset < *pp.patchXDCount);

        offset += 12;
    }

    *pp.patchPDCount = concat.totalCount() - *pp.patchPDOffset;
    return true;
}

bool SCBE_Coff::emitDirectives(const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    if (pp.directives.empty())
        return true;
    *pp.patchDROffset = concat.totalCount();
    *pp.patchDRCount  = pp.directives.length();
    concat.addString(pp.directives);
    return true;
}

bool SCBE_Coff::emitSymbolTable(const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    *pp.patchSymbolTableOffset = concat.totalCount();
    SWAG_ASSERT(pp.allSymbols.size() <= UINT32_MAX);
    *pp.patchSymbolTableCount = (uint32_t) pp.allSymbols.size();

    pp.stringTableOffset = 4;
    for (auto& symbol : pp.allSymbols)
    {
        // .Name
        if (symbol.name.length() <= 8)
        {
            // Be sure it's stuffed with 0 after the name, or we can have weird things
            // in the compiler
            concat.addU64(0);
            auto ptr = concat.getSeekPtr() - 8;
            memcpy(ptr, symbol.name.buffer, symbol.name.length());
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
        case CPUSymbolKind::Function:
            concat.addU16(pp.sectionIndexText);           // .SectionNumber
            concat.addU16(IMAGE_SYM_DTYPE_FUNCTION << 8); // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL);       // .StorageClass
            concat.addU8(0);                              // .NumberOfAuxSymbols
            break;
        case CPUSymbolKind::Extern:
            concat.addU16(0);                       // .SectionNumber
            concat.addU16(0);                       // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
            concat.addU8(0);                        // .NumberOfAuxSymbols
            break;
        case CPUSymbolKind::Custom:
            concat.addU16(symbol.sectionIdx);       // .SectionNumber
            concat.addU16(0);                       // .Type
            concat.addU8(IMAGE_SYM_CLASS_EXTERNAL); // .StorageClass
            concat.addU8(0);                        // .NumberOfAuxSymbols
            break;
        case CPUSymbolKind::GlobalString:
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

bool SCBE_Coff::emitStringTable(const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    auto& concat = pp.concat;

    concat.addU32(pp.stringTableOffset); // .Size of table in bytes + 4
    SWAG_IF_ASSERT(uint32_t subTotal = 4);
    for (auto str : pp.stringTable)
    {
        concat.addString(str->buffer, str->count);
        concat.addU8(0);
        SWAG_IF_ASSERT(subTotal += str->count + 1);
    }

    SWAG_ASSERT(subTotal == pp.stringTableOffset);
    return true;
}

bool SCBE_Coff::emitRelocationTable(Concat& concat, CPURelocationTable& cofftable, uint32_t* sectionFlags, uint16_t* count)
{
    SWAG_ASSERT(cofftable.table.size() < UINT32_MAX);
    auto tableSize = (uint32_t) cofftable.table.size();
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
        *count = (uint16_t) tableSize;
    }

    for (auto& reloc : cofftable.table)
    {
        concat.ensureSpace(4 + 4 + 2);
        concat.addU32(reloc.virtualAddress);
        concat.addU32(reloc.symbolIndex);
        concat.addU16(reloc.type);
    }

    return true;
}

bool SCBE_Coff::emitBuffer(FILE* f, const BuildParameters& buildParameters, SCBE_CPU& pp)
{
    auto module          = buildParameters.module;
    int  precompileIndex = buildParameters.precompileIndex;

    // Output the full concat buffer
    SWAG_IF_ASSERT(uint32_t totalCount = 0);
    auto bucket = pp.concat.firstBucket;
    while (bucket != pp.concat.lastBucket->nextBucket)
    {
        auto count = pp.concat.bucketCount(bucket);
        fwrite(bucket->datas, count, 1, f);
        SWAG_IF_ASSERT(totalCount += count);
        bucket = bucket->nextBucket;
    }

    SWAG_IF_ASSERT(uint32_t subTotal = 0);
    SWAG_ASSERT(totalCount == pp.concat.totalCount());

    // The global strings segment
    SWAG_ASSERT(totalCount == *pp.patchSSOffset || *pp.patchSSOffset == 0);
    for (auto oneB : pp.stringSegment.buckets)
    {
        SWAG_IF_ASSERT(totalCount += oneB.count);
        SWAG_IF_ASSERT(subTotal += oneB.count);
        fwrite(oneB.buffer, oneB.count, 1, f);
    }
    SWAG_ASSERT(subTotal == pp.stringSegment.totalCount);

    if (precompileIndex == 0)
    {
        // The global segment to store context & process infos
        SWAG_ASSERT(totalCount == *pp.patchGSOffset || *pp.patchGSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (auto oneB : pp.globalSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchGSCount || *pp.patchGSCount == 0);
        SWAG_ASSERT(subTotal == pp.globalSegment.totalCount);

        // The constant segment
        SWAG_ASSERT(totalCount == *pp.patchCSOffset || *pp.patchCSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (auto oneB : module->constantSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchCSCount || *pp.patchCSCount == 0);
        SWAG_ASSERT(subTotal == module->constantSegment.totalCount);

        // The mutable segment
        SWAG_ASSERT(totalCount == *pp.patchMSOffset || *pp.patchMSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (auto oneB : module->mutableSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchMSCount || *pp.patchMSCount == 0);
        SWAG_ASSERT(subTotal == module->mutableSegment.totalCount);

        // The tls segment
        SWAG_ASSERT(totalCount == *pp.patchTLSOffset || *pp.patchTLSOffset == 0);
        SWAG_IF_ASSERT(subTotal = 0);
        for (auto oneB : module->tlsSegment.buckets)
        {
            SWAG_IF_ASSERT(totalCount += oneB.count);
            SWAG_IF_ASSERT(subTotal += oneB.count);
            fwrite(oneB.buffer, oneB.count, 1, f);
        }
        SWAG_ASSERT(subTotal == *pp.patchTLSCount || *pp.patchTLSCount == 0);
        SWAG_ASSERT(subTotal == module->tlsSegment.totalCount);

        // The post concat buffer that contains relocation tables for CS and DS
        bucket = pp.postConcat.firstBucket;
        while (bucket != pp.postConcat.lastBucket->nextBucket)
        {
            auto count = pp.postConcat.bucketCount(bucket);
            fwrite(bucket->datas, count, 1, f);
            bucket = bucket->nextBucket;
        }
    }

    return true;
}
