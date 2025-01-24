// This header file was auto-generated using cheby
// User: daromins
// Date: 2025-01-17 11:07:21.271701
// Source map: reg_to_stream.cheby
// Command used: /home/daromins/project/code/cheby/proto/cheby.py --gen-cpp=reg_to_stream_cpp.h -i reg_to_stream.cheby
// ************************************************************

#pragma once

#include <mem_primitives.h>

namespace myModule
{

    using namespace mmpp;
    using mmpp::utils::DumpEntry;
    using mmpp::utils::DumpMap;

    //! ::RegToStream
    //!
    //! Interface registers
    struct RegToStream : MemModule<uint32_t, ByteOrdering::little, WordOrdering::big>
    {
        using MemModule::MemModule;
        [[nodiscard]] static constexpr size_t csize() noexcept
        {
            return 2048;
        }
        [[nodiscard]] size_t size() const noexcept override
        {
            return 2048;
        }

        //! Make this memory item point to another memory location
        //!
        //! @param new_base the new base address in memory that the memory item will map to
        void relocate(uint8_t* new_base) noexcept
        {
            // Construct new item in place using the new base address
            *this = RegToStream(new_base);
        }

        struct Ctrl : MemReg<RegToStream, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Ctrl, 0, 0, AccessMode::RW, bool> rst{base() + 0};     //!< (no comment provided)
            MemField<Ctrl, 1, 1, AccessMode::RW, bool> start{base() + 0};   //!< (no comment provided)
        };

        struct NumData : MemReg<RegToStream, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct Tkeep : MemReg<RegToStream, AccessMode::RW, uint64_t>
        {
            using MemReg::MemReg;
        };

        struct DataArrayItem : MemReg<RegToStream, AccessMode::RW, uint64_t>
        {
            using MemReg::MemReg;
        };
        using DataArray = MemArray<RegToStream, DataArrayItem, 100>;

        Ctrl      ctrl{base() + 0};      //!< (no comment provided)
        NumData   numData{base() + 4};   //!< (no comment provided)
        Tkeep     tkeep{base() + 8};     //!< (no comment provided)
        DataArray data{base() + 1024};   //!< (no comment provided)
    };
}

// Populate the `dump_utils` namespace with functions pertaining to `myModule`
namespace mmpp::utils
{

    // ************************************************************

    //! Dump the register and fields of `myModule::RegToStream::DataArray`
    //!
    //! @param data A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under data
    inline DumpMap dump([[maybe_unused]] const myModule::RegToStream::DataArray& data)
    {
        DumpMap res{data.base()};
        res.insert_or_assign("data[0]", DumpEntry{data[0]});
        res.insert_or_assign("data[1]", DumpEntry{data[1]});
        res.insert_or_assign("data[2]", DumpEntry{data[2]});
        res.insert_or_assign("data[3]", DumpEntry{data[3]});
        res.insert_or_assign("data[4]", DumpEntry{data[4]});
        res.insert_or_assign("data[5]", DumpEntry{data[5]});
        res.insert_or_assign("data[6]", DumpEntry{data[6]});
        res.insert_or_assign("data[7]", DumpEntry{data[7]});
        res.insert_or_assign("data[8]", DumpEntry{data[8]});
        res.insert_or_assign("data[9]", DumpEntry{data[9]});
        res.insert_or_assign("data[10]", DumpEntry{data[10]});
        res.insert_or_assign("data[11]", DumpEntry{data[11]});
        res.insert_or_assign("data[12]", DumpEntry{data[12]});
        res.insert_or_assign("data[13]", DumpEntry{data[13]});
        res.insert_or_assign("data[14]", DumpEntry{data[14]});
        res.insert_or_assign("data[15]", DumpEntry{data[15]});
        res.insert_or_assign("data[16]", DumpEntry{data[16]});
        res.insert_or_assign("data[17]", DumpEntry{data[17]});
        res.insert_or_assign("data[18]", DumpEntry{data[18]});
        res.insert_or_assign("data[19]", DumpEntry{data[19]});
        res.insert_or_assign("data[20]", DumpEntry{data[20]});
        res.insert_or_assign("data[21]", DumpEntry{data[21]});
        res.insert_or_assign("data[22]", DumpEntry{data[22]});
        res.insert_or_assign("data[23]", DumpEntry{data[23]});
        res.insert_or_assign("data[24]", DumpEntry{data[24]});
        res.insert_or_assign("data[25]", DumpEntry{data[25]});
        res.insert_or_assign("data[26]", DumpEntry{data[26]});
        res.insert_or_assign("data[27]", DumpEntry{data[27]});
        res.insert_or_assign("data[28]", DumpEntry{data[28]});
        res.insert_or_assign("data[29]", DumpEntry{data[29]});
        res.insert_or_assign("data[30]", DumpEntry{data[30]});
        res.insert_or_assign("data[31]", DumpEntry{data[31]});
        res.insert_or_assign("data[32]", DumpEntry{data[32]});
        res.insert_or_assign("data[33]", DumpEntry{data[33]});
        res.insert_or_assign("data[34]", DumpEntry{data[34]});
        res.insert_or_assign("data[35]", DumpEntry{data[35]});
        res.insert_or_assign("data[36]", DumpEntry{data[36]});
        res.insert_or_assign("data[37]", DumpEntry{data[37]});
        res.insert_or_assign("data[38]", DumpEntry{data[38]});
        res.insert_or_assign("data[39]", DumpEntry{data[39]});
        res.insert_or_assign("data[40]", DumpEntry{data[40]});
        res.insert_or_assign("data[41]", DumpEntry{data[41]});
        res.insert_or_assign("data[42]", DumpEntry{data[42]});
        res.insert_or_assign("data[43]", DumpEntry{data[43]});
        res.insert_or_assign("data[44]", DumpEntry{data[44]});
        res.insert_or_assign("data[45]", DumpEntry{data[45]});
        res.insert_or_assign("data[46]", DumpEntry{data[46]});
        res.insert_or_assign("data[47]", DumpEntry{data[47]});
        res.insert_or_assign("data[48]", DumpEntry{data[48]});
        res.insert_or_assign("data[49]", DumpEntry{data[49]});
        res.insert_or_assign("data[50]", DumpEntry{data[50]});
        res.insert_or_assign("data[51]", DumpEntry{data[51]});
        res.insert_or_assign("data[52]", DumpEntry{data[52]});
        res.insert_or_assign("data[53]", DumpEntry{data[53]});
        res.insert_or_assign("data[54]", DumpEntry{data[54]});
        res.insert_or_assign("data[55]", DumpEntry{data[55]});
        res.insert_or_assign("data[56]", DumpEntry{data[56]});
        res.insert_or_assign("data[57]", DumpEntry{data[57]});
        res.insert_or_assign("data[58]", DumpEntry{data[58]});
        res.insert_or_assign("data[59]", DumpEntry{data[59]});
        res.insert_or_assign("data[60]", DumpEntry{data[60]});
        res.insert_or_assign("data[61]", DumpEntry{data[61]});
        res.insert_or_assign("data[62]", DumpEntry{data[62]});
        res.insert_or_assign("data[63]", DumpEntry{data[63]});
        res.insert_or_assign("data[64]", DumpEntry{data[64]});
        res.insert_or_assign("data[65]", DumpEntry{data[65]});
        res.insert_or_assign("data[66]", DumpEntry{data[66]});
        res.insert_or_assign("data[67]", DumpEntry{data[67]});
        res.insert_or_assign("data[68]", DumpEntry{data[68]});
        res.insert_or_assign("data[69]", DumpEntry{data[69]});
        res.insert_or_assign("data[70]", DumpEntry{data[70]});
        res.insert_or_assign("data[71]", DumpEntry{data[71]});
        res.insert_or_assign("data[72]", DumpEntry{data[72]});
        res.insert_or_assign("data[73]", DumpEntry{data[73]});
        res.insert_or_assign("data[74]", DumpEntry{data[74]});
        res.insert_or_assign("data[75]", DumpEntry{data[75]});
        res.insert_or_assign("data[76]", DumpEntry{data[76]});
        res.insert_or_assign("data[77]", DumpEntry{data[77]});
        res.insert_or_assign("data[78]", DumpEntry{data[78]});
        res.insert_or_assign("data[79]", DumpEntry{data[79]});
        res.insert_or_assign("data[80]", DumpEntry{data[80]});
        res.insert_or_assign("data[81]", DumpEntry{data[81]});
        res.insert_or_assign("data[82]", DumpEntry{data[82]});
        res.insert_or_assign("data[83]", DumpEntry{data[83]});
        res.insert_or_assign("data[84]", DumpEntry{data[84]});
        res.insert_or_assign("data[85]", DumpEntry{data[85]});
        res.insert_or_assign("data[86]", DumpEntry{data[86]});
        res.insert_or_assign("data[87]", DumpEntry{data[87]});
        res.insert_or_assign("data[88]", DumpEntry{data[88]});
        res.insert_or_assign("data[89]", DumpEntry{data[89]});
        res.insert_or_assign("data[90]", DumpEntry{data[90]});
        res.insert_or_assign("data[91]", DumpEntry{data[91]});
        res.insert_or_assign("data[92]", DumpEntry{data[92]});
        res.insert_or_assign("data[93]", DumpEntry{data[93]});
        res.insert_or_assign("data[94]", DumpEntry{data[94]});
        res.insert_or_assign("data[95]", DumpEntry{data[95]});
        res.insert_or_assign("data[96]", DumpEntry{data[96]});
        res.insert_or_assign("data[97]", DumpEntry{data[97]});
        res.insert_or_assign("data[98]", DumpEntry{data[98]});
        res.insert_or_assign("data[99]", DumpEntry{data[99]});
        return res;
    }
    //! Dump the register and fields of `myModule::RegToStream`
    //!
    //! @param regToStream A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under regToStream
    inline DumpMap dump([[maybe_unused]] const myModule::RegToStream& regToStream)
    {
        DumpMap res{regToStream.base()};
        res.insert_or_assign("regToStream.ctrl", DumpEntry{regToStream.ctrl});
        res.insert_or_assign("regToStream.ctrl.rst", DumpEntry{regToStream.ctrl.rst});
        res.insert_or_assign("regToStream.ctrl.start", DumpEntry{regToStream.ctrl.start});
        res.insert_or_assign("regToStream.numData", DumpEntry{regToStream.numData});
        res.insert_or_assign("regToStream.tkeep", DumpEntry{regToStream.tkeep});
        res.insert_or_assign("regToStream.data[0]", DumpEntry{regToStream.data[0]});
        res.insert_or_assign("regToStream.data[1]", DumpEntry{regToStream.data[1]});
        res.insert_or_assign("regToStream.data[2]", DumpEntry{regToStream.data[2]});
        res.insert_or_assign("regToStream.data[3]", DumpEntry{regToStream.data[3]});
        res.insert_or_assign("regToStream.data[4]", DumpEntry{regToStream.data[4]});
        res.insert_or_assign("regToStream.data[5]", DumpEntry{regToStream.data[5]});
        res.insert_or_assign("regToStream.data[6]", DumpEntry{regToStream.data[6]});
        res.insert_or_assign("regToStream.data[7]", DumpEntry{regToStream.data[7]});
        res.insert_or_assign("regToStream.data[8]", DumpEntry{regToStream.data[8]});
        res.insert_or_assign("regToStream.data[9]", DumpEntry{regToStream.data[9]});
        res.insert_or_assign("regToStream.data[10]", DumpEntry{regToStream.data[10]});
        res.insert_or_assign("regToStream.data[11]", DumpEntry{regToStream.data[11]});
        res.insert_or_assign("regToStream.data[12]", DumpEntry{regToStream.data[12]});
        res.insert_or_assign("regToStream.data[13]", DumpEntry{regToStream.data[13]});
        res.insert_or_assign("regToStream.data[14]", DumpEntry{regToStream.data[14]});
        res.insert_or_assign("regToStream.data[15]", DumpEntry{regToStream.data[15]});
        res.insert_or_assign("regToStream.data[16]", DumpEntry{regToStream.data[16]});
        res.insert_or_assign("regToStream.data[17]", DumpEntry{regToStream.data[17]});
        res.insert_or_assign("regToStream.data[18]", DumpEntry{regToStream.data[18]});
        res.insert_or_assign("regToStream.data[19]", DumpEntry{regToStream.data[19]});
        res.insert_or_assign("regToStream.data[20]", DumpEntry{regToStream.data[20]});
        res.insert_or_assign("regToStream.data[21]", DumpEntry{regToStream.data[21]});
        res.insert_or_assign("regToStream.data[22]", DumpEntry{regToStream.data[22]});
        res.insert_or_assign("regToStream.data[23]", DumpEntry{regToStream.data[23]});
        res.insert_or_assign("regToStream.data[24]", DumpEntry{regToStream.data[24]});
        res.insert_or_assign("regToStream.data[25]", DumpEntry{regToStream.data[25]});
        res.insert_or_assign("regToStream.data[26]", DumpEntry{regToStream.data[26]});
        res.insert_or_assign("regToStream.data[27]", DumpEntry{regToStream.data[27]});
        res.insert_or_assign("regToStream.data[28]", DumpEntry{regToStream.data[28]});
        res.insert_or_assign("regToStream.data[29]", DumpEntry{regToStream.data[29]});
        res.insert_or_assign("regToStream.data[30]", DumpEntry{regToStream.data[30]});
        res.insert_or_assign("regToStream.data[31]", DumpEntry{regToStream.data[31]});
        res.insert_or_assign("regToStream.data[32]", DumpEntry{regToStream.data[32]});
        res.insert_or_assign("regToStream.data[33]", DumpEntry{regToStream.data[33]});
        res.insert_or_assign("regToStream.data[34]", DumpEntry{regToStream.data[34]});
        res.insert_or_assign("regToStream.data[35]", DumpEntry{regToStream.data[35]});
        res.insert_or_assign("regToStream.data[36]", DumpEntry{regToStream.data[36]});
        res.insert_or_assign("regToStream.data[37]", DumpEntry{regToStream.data[37]});
        res.insert_or_assign("regToStream.data[38]", DumpEntry{regToStream.data[38]});
        res.insert_or_assign("regToStream.data[39]", DumpEntry{regToStream.data[39]});
        res.insert_or_assign("regToStream.data[40]", DumpEntry{regToStream.data[40]});
        res.insert_or_assign("regToStream.data[41]", DumpEntry{regToStream.data[41]});
        res.insert_or_assign("regToStream.data[42]", DumpEntry{regToStream.data[42]});
        res.insert_or_assign("regToStream.data[43]", DumpEntry{regToStream.data[43]});
        res.insert_or_assign("regToStream.data[44]", DumpEntry{regToStream.data[44]});
        res.insert_or_assign("regToStream.data[45]", DumpEntry{regToStream.data[45]});
        res.insert_or_assign("regToStream.data[46]", DumpEntry{regToStream.data[46]});
        res.insert_or_assign("regToStream.data[47]", DumpEntry{regToStream.data[47]});
        res.insert_or_assign("regToStream.data[48]", DumpEntry{regToStream.data[48]});
        res.insert_or_assign("regToStream.data[49]", DumpEntry{regToStream.data[49]});
        res.insert_or_assign("regToStream.data[50]", DumpEntry{regToStream.data[50]});
        res.insert_or_assign("regToStream.data[51]", DumpEntry{regToStream.data[51]});
        res.insert_or_assign("regToStream.data[52]", DumpEntry{regToStream.data[52]});
        res.insert_or_assign("regToStream.data[53]", DumpEntry{regToStream.data[53]});
        res.insert_or_assign("regToStream.data[54]", DumpEntry{regToStream.data[54]});
        res.insert_or_assign("regToStream.data[55]", DumpEntry{regToStream.data[55]});
        res.insert_or_assign("regToStream.data[56]", DumpEntry{regToStream.data[56]});
        res.insert_or_assign("regToStream.data[57]", DumpEntry{regToStream.data[57]});
        res.insert_or_assign("regToStream.data[58]", DumpEntry{regToStream.data[58]});
        res.insert_or_assign("regToStream.data[59]", DumpEntry{regToStream.data[59]});
        res.insert_or_assign("regToStream.data[60]", DumpEntry{regToStream.data[60]});
        res.insert_or_assign("regToStream.data[61]", DumpEntry{regToStream.data[61]});
        res.insert_or_assign("regToStream.data[62]", DumpEntry{regToStream.data[62]});
        res.insert_or_assign("regToStream.data[63]", DumpEntry{regToStream.data[63]});
        res.insert_or_assign("regToStream.data[64]", DumpEntry{regToStream.data[64]});
        res.insert_or_assign("regToStream.data[65]", DumpEntry{regToStream.data[65]});
        res.insert_or_assign("regToStream.data[66]", DumpEntry{regToStream.data[66]});
        res.insert_or_assign("regToStream.data[67]", DumpEntry{regToStream.data[67]});
        res.insert_or_assign("regToStream.data[68]", DumpEntry{regToStream.data[68]});
        res.insert_or_assign("regToStream.data[69]", DumpEntry{regToStream.data[69]});
        res.insert_or_assign("regToStream.data[70]", DumpEntry{regToStream.data[70]});
        res.insert_or_assign("regToStream.data[71]", DumpEntry{regToStream.data[71]});
        res.insert_or_assign("regToStream.data[72]", DumpEntry{regToStream.data[72]});
        res.insert_or_assign("regToStream.data[73]", DumpEntry{regToStream.data[73]});
        res.insert_or_assign("regToStream.data[74]", DumpEntry{regToStream.data[74]});
        res.insert_or_assign("regToStream.data[75]", DumpEntry{regToStream.data[75]});
        res.insert_or_assign("regToStream.data[76]", DumpEntry{regToStream.data[76]});
        res.insert_or_assign("regToStream.data[77]", DumpEntry{regToStream.data[77]});
        res.insert_or_assign("regToStream.data[78]", DumpEntry{regToStream.data[78]});
        res.insert_or_assign("regToStream.data[79]", DumpEntry{regToStream.data[79]});
        res.insert_or_assign("regToStream.data[80]", DumpEntry{regToStream.data[80]});
        res.insert_or_assign("regToStream.data[81]", DumpEntry{regToStream.data[81]});
        res.insert_or_assign("regToStream.data[82]", DumpEntry{regToStream.data[82]});
        res.insert_or_assign("regToStream.data[83]", DumpEntry{regToStream.data[83]});
        res.insert_or_assign("regToStream.data[84]", DumpEntry{regToStream.data[84]});
        res.insert_or_assign("regToStream.data[85]", DumpEntry{regToStream.data[85]});
        res.insert_or_assign("regToStream.data[86]", DumpEntry{regToStream.data[86]});
        res.insert_or_assign("regToStream.data[87]", DumpEntry{regToStream.data[87]});
        res.insert_or_assign("regToStream.data[88]", DumpEntry{regToStream.data[88]});
        res.insert_or_assign("regToStream.data[89]", DumpEntry{regToStream.data[89]});
        res.insert_or_assign("regToStream.data[90]", DumpEntry{regToStream.data[90]});
        res.insert_or_assign("regToStream.data[91]", DumpEntry{regToStream.data[91]});
        res.insert_or_assign("regToStream.data[92]", DumpEntry{regToStream.data[92]});
        res.insert_or_assign("regToStream.data[93]", DumpEntry{regToStream.data[93]});
        res.insert_or_assign("regToStream.data[94]", DumpEntry{regToStream.data[94]});
        res.insert_or_assign("regToStream.data[95]", DumpEntry{regToStream.data[95]});
        res.insert_or_assign("regToStream.data[96]", DumpEntry{regToStream.data[96]});
        res.insert_or_assign("regToStream.data[97]", DumpEntry{regToStream.data[97]});
        res.insert_or_assign("regToStream.data[98]", DumpEntry{regToStream.data[98]});
        res.insert_or_assign("regToStream.data[99]", DumpEntry{regToStream.data[99]});
        return res;
    }
}
