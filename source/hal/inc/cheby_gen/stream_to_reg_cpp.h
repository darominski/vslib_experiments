// This header file was auto-generated using cheby
// User: daromins
// Date: 2025-01-17 11:07:34.499044
// Source map: stream_to_reg.cheby
// Command used: /home/daromins/project/code/cheby/proto/cheby.py --gen-cpp=stream_to_reg_cpp.h -i stream_to_reg.cheby
// ************************************************************

#pragma once

#include <mmpp.h>

namespace myModule
{

    using namespace mmpp;
    using mmpp::utils::DumpEntry;
    using mmpp::utils::DumpMap;

    //! ::StreamToReg
    //!
    //! Interface registers
    struct StreamToReg : MemModule<uint32_t, ByteOrdering::little, WordOrdering::big>
    {
        using MemModule::MemModule;
        [[nodiscard]] static constexpr size_t csize() noexcept
        {
            return 3072;
        }
        [[nodiscard]] size_t size() const noexcept override
        {
            return 3072;
        }

        //! Make this memory item point to another memory location
        //!
        //! @param new_base the new base address in memory that the memory item will map to
        void relocate(uint8_t* new_base) noexcept
        {
            // Construct new item in place using the new base address
            *this = StreamToReg(new_base);
        }

        struct Ctrl : MemReg<StreamToReg, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Ctrl, 0, 0, AccessMode::RW, bool>    resetPb{base() + 0};     //!< (no comment provided)
            MemField<Ctrl, 1, 1, AccessMode::RW, bool>    pmaInit{base() + 0};     //!< (no comment provided)
            MemField<Ctrl, 4, 6, AccessMode::RW, uint8_t> loopback{base() + 0};    //!< (no comment provided)
            MemField<Ctrl, 7, 7, AccessMode::RW, bool>    selOutput{base() + 0};   //!< (no comment provided)
        };

        struct Status : MemReg<StreamToReg, AccessMode::RO, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Status, 0, 0, AccessMode::RO, bool>   channelUp{base() + 0};     //!< (no comment provided)
            MemField<Status, 1, 1, AccessMode::RO, bool>   gtPllLock{base() + 0};     //!< (no comment provided)
            MemField<Status, 2, 2, AccessMode::RO, bool>   laneUp{base() + 0};        //!< (no comment provided)
            MemField<Status, 3, 3, AccessMode::RO, bool>   pllLocked{base() + 0};     //!< (no comment provided)
            MemField<Status, 4, 4, AccessMode::RO, bool>   softErr{base() + 0};       //!< (no comment provided)
            MemField<Status, 5, 5, AccessMode::RO, bool>   hardErr{base() + 0};       //!< (no comment provided)
            MemField<Status, 6, 6, AccessMode::RO, bool>   gtPowergood{base() + 0};   //!< (no comment provided)
            MemField<Status, 7, 7, AccessMode::RO, bool>   gtReset{base() + 0};       //!< (no comment provided)
            MemField<Status, 8, 8, AccessMode::RO, bool>   linkReset{base() + 0};     //!< (no comment provided)
            MemField<Status, 9, 9, AccessMode::RO, bool>   sysReset{base() + 0};      //!< (no comment provided)
            MemField<Status, 10, 10, AccessMode::RO, bool> almostFull{base() + 0};    //!< (no comment provided)
            MemField<Status, 11, 11, AccessMode::RO, bool> progFull{base() + 0};      //!< (no comment provided)
        };

        struct NumData : MemReg<StreamToReg, AccessMode::RO, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct DataArrayItem : MemReg<StreamToReg, AccessMode::RO, uint64_t>
        {
            using MemReg::MemReg;
        };
        using DataArray = MemArray<StreamToReg, DataArrayItem, 100>;

        struct KeepArrayItem : MemReg<StreamToReg, AccessMode::RO, uint64_t>
        {
            using MemReg::MemReg;
        };
        using KeepArray = MemArray<StreamToReg, KeepArrayItem, 100>;

        Ctrl      ctrl{base() + 0};      //!< (no comment provided)
        Status    status{base() + 4};    //!< (no comment provided)
        NumData   numData{base() + 8};   //!< (no comment provided)
        DataArray data{base() + 1024};   //!< (no comment provided)
        KeepArray keep{base() + 2048};   //!< (no comment provided)
    };
}

// Populate the `dump_utils` namespace with functions pertaining to `myModule`
namespace mmpp::utils
{

    // ************************************************************

    //! Dump the register and fields of `myModule::StreamToReg::DataArray`
    //!
    //! @param data A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under data
    inline DumpMap dump([[maybe_unused]] const myModule::StreamToReg::DataArray& data)
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

    //! Dump the register and fields of `myModule::StreamToReg::KeepArray`
    //!
    //! @param keep A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under keep
    inline DumpMap dump([[maybe_unused]] const myModule::StreamToReg::KeepArray& keep)
    {
        DumpMap res{keep.base()};
        res.insert_or_assign("keep[0]", DumpEntry{keep[0]});
        res.insert_or_assign("keep[1]", DumpEntry{keep[1]});
        res.insert_or_assign("keep[2]", DumpEntry{keep[2]});
        res.insert_or_assign("keep[3]", DumpEntry{keep[3]});
        res.insert_or_assign("keep[4]", DumpEntry{keep[4]});
        res.insert_or_assign("keep[5]", DumpEntry{keep[5]});
        res.insert_or_assign("keep[6]", DumpEntry{keep[6]});
        res.insert_or_assign("keep[7]", DumpEntry{keep[7]});
        res.insert_or_assign("keep[8]", DumpEntry{keep[8]});
        res.insert_or_assign("keep[9]", DumpEntry{keep[9]});
        res.insert_or_assign("keep[10]", DumpEntry{keep[10]});
        res.insert_or_assign("keep[11]", DumpEntry{keep[11]});
        res.insert_or_assign("keep[12]", DumpEntry{keep[12]});
        res.insert_or_assign("keep[13]", DumpEntry{keep[13]});
        res.insert_or_assign("keep[14]", DumpEntry{keep[14]});
        res.insert_or_assign("keep[15]", DumpEntry{keep[15]});
        res.insert_or_assign("keep[16]", DumpEntry{keep[16]});
        res.insert_or_assign("keep[17]", DumpEntry{keep[17]});
        res.insert_or_assign("keep[18]", DumpEntry{keep[18]});
        res.insert_or_assign("keep[19]", DumpEntry{keep[19]});
        res.insert_or_assign("keep[20]", DumpEntry{keep[20]});
        res.insert_or_assign("keep[21]", DumpEntry{keep[21]});
        res.insert_or_assign("keep[22]", DumpEntry{keep[22]});
        res.insert_or_assign("keep[23]", DumpEntry{keep[23]});
        res.insert_or_assign("keep[24]", DumpEntry{keep[24]});
        res.insert_or_assign("keep[25]", DumpEntry{keep[25]});
        res.insert_or_assign("keep[26]", DumpEntry{keep[26]});
        res.insert_or_assign("keep[27]", DumpEntry{keep[27]});
        res.insert_or_assign("keep[28]", DumpEntry{keep[28]});
        res.insert_or_assign("keep[29]", DumpEntry{keep[29]});
        res.insert_or_assign("keep[30]", DumpEntry{keep[30]});
        res.insert_or_assign("keep[31]", DumpEntry{keep[31]});
        res.insert_or_assign("keep[32]", DumpEntry{keep[32]});
        res.insert_or_assign("keep[33]", DumpEntry{keep[33]});
        res.insert_or_assign("keep[34]", DumpEntry{keep[34]});
        res.insert_or_assign("keep[35]", DumpEntry{keep[35]});
        res.insert_or_assign("keep[36]", DumpEntry{keep[36]});
        res.insert_or_assign("keep[37]", DumpEntry{keep[37]});
        res.insert_or_assign("keep[38]", DumpEntry{keep[38]});
        res.insert_or_assign("keep[39]", DumpEntry{keep[39]});
        res.insert_or_assign("keep[40]", DumpEntry{keep[40]});
        res.insert_or_assign("keep[41]", DumpEntry{keep[41]});
        res.insert_or_assign("keep[42]", DumpEntry{keep[42]});
        res.insert_or_assign("keep[43]", DumpEntry{keep[43]});
        res.insert_or_assign("keep[44]", DumpEntry{keep[44]});
        res.insert_or_assign("keep[45]", DumpEntry{keep[45]});
        res.insert_or_assign("keep[46]", DumpEntry{keep[46]});
        res.insert_or_assign("keep[47]", DumpEntry{keep[47]});
        res.insert_or_assign("keep[48]", DumpEntry{keep[48]});
        res.insert_or_assign("keep[49]", DumpEntry{keep[49]});
        res.insert_or_assign("keep[50]", DumpEntry{keep[50]});
        res.insert_or_assign("keep[51]", DumpEntry{keep[51]});
        res.insert_or_assign("keep[52]", DumpEntry{keep[52]});
        res.insert_or_assign("keep[53]", DumpEntry{keep[53]});
        res.insert_or_assign("keep[54]", DumpEntry{keep[54]});
        res.insert_or_assign("keep[55]", DumpEntry{keep[55]});
        res.insert_or_assign("keep[56]", DumpEntry{keep[56]});
        res.insert_or_assign("keep[57]", DumpEntry{keep[57]});
        res.insert_or_assign("keep[58]", DumpEntry{keep[58]});
        res.insert_or_assign("keep[59]", DumpEntry{keep[59]});
        res.insert_or_assign("keep[60]", DumpEntry{keep[60]});
        res.insert_or_assign("keep[61]", DumpEntry{keep[61]});
        res.insert_or_assign("keep[62]", DumpEntry{keep[62]});
        res.insert_or_assign("keep[63]", DumpEntry{keep[63]});
        res.insert_or_assign("keep[64]", DumpEntry{keep[64]});
        res.insert_or_assign("keep[65]", DumpEntry{keep[65]});
        res.insert_or_assign("keep[66]", DumpEntry{keep[66]});
        res.insert_or_assign("keep[67]", DumpEntry{keep[67]});
        res.insert_or_assign("keep[68]", DumpEntry{keep[68]});
        res.insert_or_assign("keep[69]", DumpEntry{keep[69]});
        res.insert_or_assign("keep[70]", DumpEntry{keep[70]});
        res.insert_or_assign("keep[71]", DumpEntry{keep[71]});
        res.insert_or_assign("keep[72]", DumpEntry{keep[72]});
        res.insert_or_assign("keep[73]", DumpEntry{keep[73]});
        res.insert_or_assign("keep[74]", DumpEntry{keep[74]});
        res.insert_or_assign("keep[75]", DumpEntry{keep[75]});
        res.insert_or_assign("keep[76]", DumpEntry{keep[76]});
        res.insert_or_assign("keep[77]", DumpEntry{keep[77]});
        res.insert_or_assign("keep[78]", DumpEntry{keep[78]});
        res.insert_or_assign("keep[79]", DumpEntry{keep[79]});
        res.insert_or_assign("keep[80]", DumpEntry{keep[80]});
        res.insert_or_assign("keep[81]", DumpEntry{keep[81]});
        res.insert_or_assign("keep[82]", DumpEntry{keep[82]});
        res.insert_or_assign("keep[83]", DumpEntry{keep[83]});
        res.insert_or_assign("keep[84]", DumpEntry{keep[84]});
        res.insert_or_assign("keep[85]", DumpEntry{keep[85]});
        res.insert_or_assign("keep[86]", DumpEntry{keep[86]});
        res.insert_or_assign("keep[87]", DumpEntry{keep[87]});
        res.insert_or_assign("keep[88]", DumpEntry{keep[88]});
        res.insert_or_assign("keep[89]", DumpEntry{keep[89]});
        res.insert_or_assign("keep[90]", DumpEntry{keep[90]});
        res.insert_or_assign("keep[91]", DumpEntry{keep[91]});
        res.insert_or_assign("keep[92]", DumpEntry{keep[92]});
        res.insert_or_assign("keep[93]", DumpEntry{keep[93]});
        res.insert_or_assign("keep[94]", DumpEntry{keep[94]});
        res.insert_or_assign("keep[95]", DumpEntry{keep[95]});
        res.insert_or_assign("keep[96]", DumpEntry{keep[96]});
        res.insert_or_assign("keep[97]", DumpEntry{keep[97]});
        res.insert_or_assign("keep[98]", DumpEntry{keep[98]});
        res.insert_or_assign("keep[99]", DumpEntry{keep[99]});
        return res;
    }
    //! Dump the register and fields of `myModule::StreamToReg`
    //!
    //! @param streamToReg A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under streamToReg
    inline DumpMap dump([[maybe_unused]] const myModule::StreamToReg& streamToReg)
    {
        DumpMap res{streamToReg.base()};
        res.insert_or_assign("streamToReg.ctrl", DumpEntry{streamToReg.ctrl});
        res.insert_or_assign("streamToReg.ctrl.resetPb", DumpEntry{streamToReg.ctrl.resetPb});
        res.insert_or_assign("streamToReg.ctrl.pmaInit", DumpEntry{streamToReg.ctrl.pmaInit});
        res.insert_or_assign("streamToReg.ctrl.loopback", DumpEntry{streamToReg.ctrl.loopback});
        res.insert_or_assign("streamToReg.ctrl.selOutput", DumpEntry{streamToReg.ctrl.selOutput});
        res.insert_or_assign("streamToReg.status", DumpEntry{streamToReg.status});
        res.insert_or_assign("streamToReg.status.channelUp", DumpEntry{streamToReg.status.channelUp});
        res.insert_or_assign("streamToReg.status.gtPllLock", DumpEntry{streamToReg.status.gtPllLock});
        res.insert_or_assign("streamToReg.status.laneUp", DumpEntry{streamToReg.status.laneUp});
        res.insert_or_assign("streamToReg.status.pllLocked", DumpEntry{streamToReg.status.pllLocked});
        res.insert_or_assign("streamToReg.status.softErr", DumpEntry{streamToReg.status.softErr});
        res.insert_or_assign("streamToReg.status.hardErr", DumpEntry{streamToReg.status.hardErr});
        res.insert_or_assign("streamToReg.status.gtPowergood", DumpEntry{streamToReg.status.gtPowergood});
        res.insert_or_assign("streamToReg.status.gtReset", DumpEntry{streamToReg.status.gtReset});
        res.insert_or_assign("streamToReg.status.linkReset", DumpEntry{streamToReg.status.linkReset});
        res.insert_or_assign("streamToReg.status.sysReset", DumpEntry{streamToReg.status.sysReset});
        res.insert_or_assign("streamToReg.status.almostFull", DumpEntry{streamToReg.status.almostFull});
        res.insert_or_assign("streamToReg.status.progFull", DumpEntry{streamToReg.status.progFull});
        res.insert_or_assign("streamToReg.numData", DumpEntry{streamToReg.numData});
        res.insert_or_assign("streamToReg.data[0]", DumpEntry{streamToReg.data[0]});
        res.insert_or_assign("streamToReg.data[1]", DumpEntry{streamToReg.data[1]});
        res.insert_or_assign("streamToReg.data[2]", DumpEntry{streamToReg.data[2]});
        res.insert_or_assign("streamToReg.data[3]", DumpEntry{streamToReg.data[3]});
        res.insert_or_assign("streamToReg.data[4]", DumpEntry{streamToReg.data[4]});
        res.insert_or_assign("streamToReg.data[5]", DumpEntry{streamToReg.data[5]});
        res.insert_or_assign("streamToReg.data[6]", DumpEntry{streamToReg.data[6]});
        res.insert_or_assign("streamToReg.data[7]", DumpEntry{streamToReg.data[7]});
        res.insert_or_assign("streamToReg.data[8]", DumpEntry{streamToReg.data[8]});
        res.insert_or_assign("streamToReg.data[9]", DumpEntry{streamToReg.data[9]});
        res.insert_or_assign("streamToReg.data[10]", DumpEntry{streamToReg.data[10]});
        res.insert_or_assign("streamToReg.data[11]", DumpEntry{streamToReg.data[11]});
        res.insert_or_assign("streamToReg.data[12]", DumpEntry{streamToReg.data[12]});
        res.insert_or_assign("streamToReg.data[13]", DumpEntry{streamToReg.data[13]});
        res.insert_or_assign("streamToReg.data[14]", DumpEntry{streamToReg.data[14]});
        res.insert_or_assign("streamToReg.data[15]", DumpEntry{streamToReg.data[15]});
        res.insert_or_assign("streamToReg.data[16]", DumpEntry{streamToReg.data[16]});
        res.insert_or_assign("streamToReg.data[17]", DumpEntry{streamToReg.data[17]});
        res.insert_or_assign("streamToReg.data[18]", DumpEntry{streamToReg.data[18]});
        res.insert_or_assign("streamToReg.data[19]", DumpEntry{streamToReg.data[19]});
        res.insert_or_assign("streamToReg.data[20]", DumpEntry{streamToReg.data[20]});
        res.insert_or_assign("streamToReg.data[21]", DumpEntry{streamToReg.data[21]});
        res.insert_or_assign("streamToReg.data[22]", DumpEntry{streamToReg.data[22]});
        res.insert_or_assign("streamToReg.data[23]", DumpEntry{streamToReg.data[23]});
        res.insert_or_assign("streamToReg.data[24]", DumpEntry{streamToReg.data[24]});
        res.insert_or_assign("streamToReg.data[25]", DumpEntry{streamToReg.data[25]});
        res.insert_or_assign("streamToReg.data[26]", DumpEntry{streamToReg.data[26]});
        res.insert_or_assign("streamToReg.data[27]", DumpEntry{streamToReg.data[27]});
        res.insert_or_assign("streamToReg.data[28]", DumpEntry{streamToReg.data[28]});
        res.insert_or_assign("streamToReg.data[29]", DumpEntry{streamToReg.data[29]});
        res.insert_or_assign("streamToReg.data[30]", DumpEntry{streamToReg.data[30]});
        res.insert_or_assign("streamToReg.data[31]", DumpEntry{streamToReg.data[31]});
        res.insert_or_assign("streamToReg.data[32]", DumpEntry{streamToReg.data[32]});
        res.insert_or_assign("streamToReg.data[33]", DumpEntry{streamToReg.data[33]});
        res.insert_or_assign("streamToReg.data[34]", DumpEntry{streamToReg.data[34]});
        res.insert_or_assign("streamToReg.data[35]", DumpEntry{streamToReg.data[35]});
        res.insert_or_assign("streamToReg.data[36]", DumpEntry{streamToReg.data[36]});
        res.insert_or_assign("streamToReg.data[37]", DumpEntry{streamToReg.data[37]});
        res.insert_or_assign("streamToReg.data[38]", DumpEntry{streamToReg.data[38]});
        res.insert_or_assign("streamToReg.data[39]", DumpEntry{streamToReg.data[39]});
        res.insert_or_assign("streamToReg.data[40]", DumpEntry{streamToReg.data[40]});
        res.insert_or_assign("streamToReg.data[41]", DumpEntry{streamToReg.data[41]});
        res.insert_or_assign("streamToReg.data[42]", DumpEntry{streamToReg.data[42]});
        res.insert_or_assign("streamToReg.data[43]", DumpEntry{streamToReg.data[43]});
        res.insert_or_assign("streamToReg.data[44]", DumpEntry{streamToReg.data[44]});
        res.insert_or_assign("streamToReg.data[45]", DumpEntry{streamToReg.data[45]});
        res.insert_or_assign("streamToReg.data[46]", DumpEntry{streamToReg.data[46]});
        res.insert_or_assign("streamToReg.data[47]", DumpEntry{streamToReg.data[47]});
        res.insert_or_assign("streamToReg.data[48]", DumpEntry{streamToReg.data[48]});
        res.insert_or_assign("streamToReg.data[49]", DumpEntry{streamToReg.data[49]});
        res.insert_or_assign("streamToReg.data[50]", DumpEntry{streamToReg.data[50]});
        res.insert_or_assign("streamToReg.data[51]", DumpEntry{streamToReg.data[51]});
        res.insert_or_assign("streamToReg.data[52]", DumpEntry{streamToReg.data[52]});
        res.insert_or_assign("streamToReg.data[53]", DumpEntry{streamToReg.data[53]});
        res.insert_or_assign("streamToReg.data[54]", DumpEntry{streamToReg.data[54]});
        res.insert_or_assign("streamToReg.data[55]", DumpEntry{streamToReg.data[55]});
        res.insert_or_assign("streamToReg.data[56]", DumpEntry{streamToReg.data[56]});
        res.insert_or_assign("streamToReg.data[57]", DumpEntry{streamToReg.data[57]});
        res.insert_or_assign("streamToReg.data[58]", DumpEntry{streamToReg.data[58]});
        res.insert_or_assign("streamToReg.data[59]", DumpEntry{streamToReg.data[59]});
        res.insert_or_assign("streamToReg.data[60]", DumpEntry{streamToReg.data[60]});
        res.insert_or_assign("streamToReg.data[61]", DumpEntry{streamToReg.data[61]});
        res.insert_or_assign("streamToReg.data[62]", DumpEntry{streamToReg.data[62]});
        res.insert_or_assign("streamToReg.data[63]", DumpEntry{streamToReg.data[63]});
        res.insert_or_assign("streamToReg.data[64]", DumpEntry{streamToReg.data[64]});
        res.insert_or_assign("streamToReg.data[65]", DumpEntry{streamToReg.data[65]});
        res.insert_or_assign("streamToReg.data[66]", DumpEntry{streamToReg.data[66]});
        res.insert_or_assign("streamToReg.data[67]", DumpEntry{streamToReg.data[67]});
        res.insert_or_assign("streamToReg.data[68]", DumpEntry{streamToReg.data[68]});
        res.insert_or_assign("streamToReg.data[69]", DumpEntry{streamToReg.data[69]});
        res.insert_or_assign("streamToReg.data[70]", DumpEntry{streamToReg.data[70]});
        res.insert_or_assign("streamToReg.data[71]", DumpEntry{streamToReg.data[71]});
        res.insert_or_assign("streamToReg.data[72]", DumpEntry{streamToReg.data[72]});
        res.insert_or_assign("streamToReg.data[73]", DumpEntry{streamToReg.data[73]});
        res.insert_or_assign("streamToReg.data[74]", DumpEntry{streamToReg.data[74]});
        res.insert_or_assign("streamToReg.data[75]", DumpEntry{streamToReg.data[75]});
        res.insert_or_assign("streamToReg.data[76]", DumpEntry{streamToReg.data[76]});
        res.insert_or_assign("streamToReg.data[77]", DumpEntry{streamToReg.data[77]});
        res.insert_or_assign("streamToReg.data[78]", DumpEntry{streamToReg.data[78]});
        res.insert_or_assign("streamToReg.data[79]", DumpEntry{streamToReg.data[79]});
        res.insert_or_assign("streamToReg.data[80]", DumpEntry{streamToReg.data[80]});
        res.insert_or_assign("streamToReg.data[81]", DumpEntry{streamToReg.data[81]});
        res.insert_or_assign("streamToReg.data[82]", DumpEntry{streamToReg.data[82]});
        res.insert_or_assign("streamToReg.data[83]", DumpEntry{streamToReg.data[83]});
        res.insert_or_assign("streamToReg.data[84]", DumpEntry{streamToReg.data[84]});
        res.insert_or_assign("streamToReg.data[85]", DumpEntry{streamToReg.data[85]});
        res.insert_or_assign("streamToReg.data[86]", DumpEntry{streamToReg.data[86]});
        res.insert_or_assign("streamToReg.data[87]", DumpEntry{streamToReg.data[87]});
        res.insert_or_assign("streamToReg.data[88]", DumpEntry{streamToReg.data[88]});
        res.insert_or_assign("streamToReg.data[89]", DumpEntry{streamToReg.data[89]});
        res.insert_or_assign("streamToReg.data[90]", DumpEntry{streamToReg.data[90]});
        res.insert_or_assign("streamToReg.data[91]", DumpEntry{streamToReg.data[91]});
        res.insert_or_assign("streamToReg.data[92]", DumpEntry{streamToReg.data[92]});
        res.insert_or_assign("streamToReg.data[93]", DumpEntry{streamToReg.data[93]});
        res.insert_or_assign("streamToReg.data[94]", DumpEntry{streamToReg.data[94]});
        res.insert_or_assign("streamToReg.data[95]", DumpEntry{streamToReg.data[95]});
        res.insert_or_assign("streamToReg.data[96]", DumpEntry{streamToReg.data[96]});
        res.insert_or_assign("streamToReg.data[97]", DumpEntry{streamToReg.data[97]});
        res.insert_or_assign("streamToReg.data[98]", DumpEntry{streamToReg.data[98]});
        res.insert_or_assign("streamToReg.data[99]", DumpEntry{streamToReg.data[99]});
        res.insert_or_assign("streamToReg.keep[0]", DumpEntry{streamToReg.keep[0]});
        res.insert_or_assign("streamToReg.keep[1]", DumpEntry{streamToReg.keep[1]});
        res.insert_or_assign("streamToReg.keep[2]", DumpEntry{streamToReg.keep[2]});
        res.insert_or_assign("streamToReg.keep[3]", DumpEntry{streamToReg.keep[3]});
        res.insert_or_assign("streamToReg.keep[4]", DumpEntry{streamToReg.keep[4]});
        res.insert_or_assign("streamToReg.keep[5]", DumpEntry{streamToReg.keep[5]});
        res.insert_or_assign("streamToReg.keep[6]", DumpEntry{streamToReg.keep[6]});
        res.insert_or_assign("streamToReg.keep[7]", DumpEntry{streamToReg.keep[7]});
        res.insert_or_assign("streamToReg.keep[8]", DumpEntry{streamToReg.keep[8]});
        res.insert_or_assign("streamToReg.keep[9]", DumpEntry{streamToReg.keep[9]});
        res.insert_or_assign("streamToReg.keep[10]", DumpEntry{streamToReg.keep[10]});
        res.insert_or_assign("streamToReg.keep[11]", DumpEntry{streamToReg.keep[11]});
        res.insert_or_assign("streamToReg.keep[12]", DumpEntry{streamToReg.keep[12]});
        res.insert_or_assign("streamToReg.keep[13]", DumpEntry{streamToReg.keep[13]});
        res.insert_or_assign("streamToReg.keep[14]", DumpEntry{streamToReg.keep[14]});
        res.insert_or_assign("streamToReg.keep[15]", DumpEntry{streamToReg.keep[15]});
        res.insert_or_assign("streamToReg.keep[16]", DumpEntry{streamToReg.keep[16]});
        res.insert_or_assign("streamToReg.keep[17]", DumpEntry{streamToReg.keep[17]});
        res.insert_or_assign("streamToReg.keep[18]", DumpEntry{streamToReg.keep[18]});
        res.insert_or_assign("streamToReg.keep[19]", DumpEntry{streamToReg.keep[19]});
        res.insert_or_assign("streamToReg.keep[20]", DumpEntry{streamToReg.keep[20]});
        res.insert_or_assign("streamToReg.keep[21]", DumpEntry{streamToReg.keep[21]});
        res.insert_or_assign("streamToReg.keep[22]", DumpEntry{streamToReg.keep[22]});
        res.insert_or_assign("streamToReg.keep[23]", DumpEntry{streamToReg.keep[23]});
        res.insert_or_assign("streamToReg.keep[24]", DumpEntry{streamToReg.keep[24]});
        res.insert_or_assign("streamToReg.keep[25]", DumpEntry{streamToReg.keep[25]});
        res.insert_or_assign("streamToReg.keep[26]", DumpEntry{streamToReg.keep[26]});
        res.insert_or_assign("streamToReg.keep[27]", DumpEntry{streamToReg.keep[27]});
        res.insert_or_assign("streamToReg.keep[28]", DumpEntry{streamToReg.keep[28]});
        res.insert_or_assign("streamToReg.keep[29]", DumpEntry{streamToReg.keep[29]});
        res.insert_or_assign("streamToReg.keep[30]", DumpEntry{streamToReg.keep[30]});
        res.insert_or_assign("streamToReg.keep[31]", DumpEntry{streamToReg.keep[31]});
        res.insert_or_assign("streamToReg.keep[32]", DumpEntry{streamToReg.keep[32]});
        res.insert_or_assign("streamToReg.keep[33]", DumpEntry{streamToReg.keep[33]});
        res.insert_or_assign("streamToReg.keep[34]", DumpEntry{streamToReg.keep[34]});
        res.insert_or_assign("streamToReg.keep[35]", DumpEntry{streamToReg.keep[35]});
        res.insert_or_assign("streamToReg.keep[36]", DumpEntry{streamToReg.keep[36]});
        res.insert_or_assign("streamToReg.keep[37]", DumpEntry{streamToReg.keep[37]});
        res.insert_or_assign("streamToReg.keep[38]", DumpEntry{streamToReg.keep[38]});
        res.insert_or_assign("streamToReg.keep[39]", DumpEntry{streamToReg.keep[39]});
        res.insert_or_assign("streamToReg.keep[40]", DumpEntry{streamToReg.keep[40]});
        res.insert_or_assign("streamToReg.keep[41]", DumpEntry{streamToReg.keep[41]});
        res.insert_or_assign("streamToReg.keep[42]", DumpEntry{streamToReg.keep[42]});
        res.insert_or_assign("streamToReg.keep[43]", DumpEntry{streamToReg.keep[43]});
        res.insert_or_assign("streamToReg.keep[44]", DumpEntry{streamToReg.keep[44]});
        res.insert_or_assign("streamToReg.keep[45]", DumpEntry{streamToReg.keep[45]});
        res.insert_or_assign("streamToReg.keep[46]", DumpEntry{streamToReg.keep[46]});
        res.insert_or_assign("streamToReg.keep[47]", DumpEntry{streamToReg.keep[47]});
        res.insert_or_assign("streamToReg.keep[48]", DumpEntry{streamToReg.keep[48]});
        res.insert_or_assign("streamToReg.keep[49]", DumpEntry{streamToReg.keep[49]});
        res.insert_or_assign("streamToReg.keep[50]", DumpEntry{streamToReg.keep[50]});
        res.insert_or_assign("streamToReg.keep[51]", DumpEntry{streamToReg.keep[51]});
        res.insert_or_assign("streamToReg.keep[52]", DumpEntry{streamToReg.keep[52]});
        res.insert_or_assign("streamToReg.keep[53]", DumpEntry{streamToReg.keep[53]});
        res.insert_or_assign("streamToReg.keep[54]", DumpEntry{streamToReg.keep[54]});
        res.insert_or_assign("streamToReg.keep[55]", DumpEntry{streamToReg.keep[55]});
        res.insert_or_assign("streamToReg.keep[56]", DumpEntry{streamToReg.keep[56]});
        res.insert_or_assign("streamToReg.keep[57]", DumpEntry{streamToReg.keep[57]});
        res.insert_or_assign("streamToReg.keep[58]", DumpEntry{streamToReg.keep[58]});
        res.insert_or_assign("streamToReg.keep[59]", DumpEntry{streamToReg.keep[59]});
        res.insert_or_assign("streamToReg.keep[60]", DumpEntry{streamToReg.keep[60]});
        res.insert_or_assign("streamToReg.keep[61]", DumpEntry{streamToReg.keep[61]});
        res.insert_or_assign("streamToReg.keep[62]", DumpEntry{streamToReg.keep[62]});
        res.insert_or_assign("streamToReg.keep[63]", DumpEntry{streamToReg.keep[63]});
        res.insert_or_assign("streamToReg.keep[64]", DumpEntry{streamToReg.keep[64]});
        res.insert_or_assign("streamToReg.keep[65]", DumpEntry{streamToReg.keep[65]});
        res.insert_or_assign("streamToReg.keep[66]", DumpEntry{streamToReg.keep[66]});
        res.insert_or_assign("streamToReg.keep[67]", DumpEntry{streamToReg.keep[67]});
        res.insert_or_assign("streamToReg.keep[68]", DumpEntry{streamToReg.keep[68]});
        res.insert_or_assign("streamToReg.keep[69]", DumpEntry{streamToReg.keep[69]});
        res.insert_or_assign("streamToReg.keep[70]", DumpEntry{streamToReg.keep[70]});
        res.insert_or_assign("streamToReg.keep[71]", DumpEntry{streamToReg.keep[71]});
        res.insert_or_assign("streamToReg.keep[72]", DumpEntry{streamToReg.keep[72]});
        res.insert_or_assign("streamToReg.keep[73]", DumpEntry{streamToReg.keep[73]});
        res.insert_or_assign("streamToReg.keep[74]", DumpEntry{streamToReg.keep[74]});
        res.insert_or_assign("streamToReg.keep[75]", DumpEntry{streamToReg.keep[75]});
        res.insert_or_assign("streamToReg.keep[76]", DumpEntry{streamToReg.keep[76]});
        res.insert_or_assign("streamToReg.keep[77]", DumpEntry{streamToReg.keep[77]});
        res.insert_or_assign("streamToReg.keep[78]", DumpEntry{streamToReg.keep[78]});
        res.insert_or_assign("streamToReg.keep[79]", DumpEntry{streamToReg.keep[79]});
        res.insert_or_assign("streamToReg.keep[80]", DumpEntry{streamToReg.keep[80]});
        res.insert_or_assign("streamToReg.keep[81]", DumpEntry{streamToReg.keep[81]});
        res.insert_or_assign("streamToReg.keep[82]", DumpEntry{streamToReg.keep[82]});
        res.insert_or_assign("streamToReg.keep[83]", DumpEntry{streamToReg.keep[83]});
        res.insert_or_assign("streamToReg.keep[84]", DumpEntry{streamToReg.keep[84]});
        res.insert_or_assign("streamToReg.keep[85]", DumpEntry{streamToReg.keep[85]});
        res.insert_or_assign("streamToReg.keep[86]", DumpEntry{streamToReg.keep[86]});
        res.insert_or_assign("streamToReg.keep[87]", DumpEntry{streamToReg.keep[87]});
        res.insert_or_assign("streamToReg.keep[88]", DumpEntry{streamToReg.keep[88]});
        res.insert_or_assign("streamToReg.keep[89]", DumpEntry{streamToReg.keep[89]});
        res.insert_or_assign("streamToReg.keep[90]", DumpEntry{streamToReg.keep[90]});
        res.insert_or_assign("streamToReg.keep[91]", DumpEntry{streamToReg.keep[91]});
        res.insert_or_assign("streamToReg.keep[92]", DumpEntry{streamToReg.keep[92]});
        res.insert_or_assign("streamToReg.keep[93]", DumpEntry{streamToReg.keep[93]});
        res.insert_or_assign("streamToReg.keep[94]", DumpEntry{streamToReg.keep[94]});
        res.insert_or_assign("streamToReg.keep[95]", DumpEntry{streamToReg.keep[95]});
        res.insert_or_assign("streamToReg.keep[96]", DumpEntry{streamToReg.keep[96]});
        res.insert_or_assign("streamToReg.keep[97]", DumpEntry{streamToReg.keep[97]});
        res.insert_or_assign("streamToReg.keep[98]", DumpEntry{streamToReg.keep[98]});
        res.insert_or_assign("streamToReg.keep[99]", DumpEntry{streamToReg.keep[99]});
        return res;
    }
}
