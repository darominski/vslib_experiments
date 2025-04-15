// This header file was auto-generated using cheby
// User: daromins
// Date: 2025-04-08 15:21:12.154910
// Source map: reg_to_stream.cheby
// Command used: /home/daromins/project/code/cheby/proto/cheby.py --gen-cpp=reg_to_stream.hpp -i reg_to_stream.cheby
//
// It is meant to be used in conjunction with the MemMap++ library (mmpp):
//     https://gitlab.cern.ch/czrounba/mmpp
//
// ************************************************************

#pragma once

#include <mmpp.h>

namespace ipCores
{
    using namespace mmpp;

    //! ::RegToStream
    //!
    //! Interface registers
    struct RegToStream : MemModule<2048, uint32_t, attributes::ByteOrdering::little, attributes::WordOrdering::big>
    {
        // No version information provided.

        using MemModule::MemModule;

        //! Make this memory item point to another memory location
        //!
        //! @param new_base the new base address in memory that the memory item will map to
        void relocate(uint8_t* new_base) noexcept
        {
            // Construct new item in place using the new base address
            *this = RegToStream(new_base);
        }

        struct Ctrl : MemReg<RegToStream, 4, attributes::AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Ctrl, 0, 0, attributes::AccessMode::RW, bool> rst{base() + 0};     //!< Reset the reg_to_stream IP.
            MemField<Ctrl, 1, 1, attributes::AccessMode::RW, bool> start{base() + 0};   //!< Starts the data transfer.
        };

        struct NumData : MemReg<RegToStream, 4, attributes::AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct Tkeep : MemReg<RegToStream, 4, attributes::AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct DataArrayItem : MemReg<RegToStream, 8, attributes::AccessMode::RW, uint64_t>
        {
            using MemReg::MemReg;
        };
        using DataArray = MemArray<RegToStream, DataArrayItem, 100, 8>;

        Ctrl    ctrl{base() + 0};      //!< Control register
        NumData numData{base() + 4};   //!< How many data words should be sent.
        //! Last tkeep to be send. Only the 8 lower bits are used.
        Tkeep     tkeep{base() + 8};
        DataArray data{base() + 1024};   //!< Data received from Aurora
    };
}

// Populate the `mmpp::utils` namespace with functions pertaining to `ipCores`
namespace mmpp::utils
{

    // ************************************************************

    //! Dump the register and fields of `ipCores::RegToStream::DataArray`
    //!
    //! @param data A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under data
    inline DumpMap dump([[maybe_unused]] const ipCores::RegToStream::DataArray& data)
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
        res.insert_or_assign("data[100]", DumpEntry{data[100]});
        res.insert_or_assign("data[101]", DumpEntry{data[101]});
        res.insert_or_assign("data[102]", DumpEntry{data[102]});
        res.insert_or_assign("data[103]", DumpEntry{data[103]});
        res.insert_or_assign("data[104]", DumpEntry{data[104]});
        res.insert_or_assign("data[105]", DumpEntry{data[105]});
        res.insert_or_assign("data[106]", DumpEntry{data[106]});
        res.insert_or_assign("data[107]", DumpEntry{data[107]});
        res.insert_or_assign("data[108]", DumpEntry{data[108]});
        res.insert_or_assign("data[109]", DumpEntry{data[109]});
        res.insert_or_assign("data[110]", DumpEntry{data[110]});
        res.insert_or_assign("data[111]", DumpEntry{data[111]});
        res.insert_or_assign("data[112]", DumpEntry{data[112]});
        res.insert_or_assign("data[113]", DumpEntry{data[113]});
        res.insert_or_assign("data[114]", DumpEntry{data[114]});
        res.insert_or_assign("data[115]", DumpEntry{data[115]});
        res.insert_or_assign("data[116]", DumpEntry{data[116]});
        res.insert_or_assign("data[117]", DumpEntry{data[117]});
        res.insert_or_assign("data[118]", DumpEntry{data[118]});
        res.insert_or_assign("data[119]", DumpEntry{data[119]});
        res.insert_or_assign("data[120]", DumpEntry{data[120]});
        res.insert_or_assign("data[121]", DumpEntry{data[121]});
        res.insert_or_assign("data[122]", DumpEntry{data[122]});
        res.insert_or_assign("data[123]", DumpEntry{data[123]});
        res.insert_or_assign("data[124]", DumpEntry{data[124]});
        res.insert_or_assign("data[125]", DumpEntry{data[125]});
        res.insert_or_assign("data[126]", DumpEntry{data[126]});
        res.insert_or_assign("data[127]", DumpEntry{data[127]});
        res.insert_or_assign("data[128]", DumpEntry{data[128]});
        res.insert_or_assign("data[129]", DumpEntry{data[129]});
        res.insert_or_assign("data[130]", DumpEntry{data[130]});
        res.insert_or_assign("data[131]", DumpEntry{data[131]});
        res.insert_or_assign("data[132]", DumpEntry{data[132]});
        res.insert_or_assign("data[133]", DumpEntry{data[133]});
        res.insert_or_assign("data[134]", DumpEntry{data[134]});
        res.insert_or_assign("data[135]", DumpEntry{data[135]});
        res.insert_or_assign("data[136]", DumpEntry{data[136]});
        res.insert_or_assign("data[137]", DumpEntry{data[137]});
        res.insert_or_assign("data[138]", DumpEntry{data[138]});
        res.insert_or_assign("data[139]", DumpEntry{data[139]});
        res.insert_or_assign("data[140]", DumpEntry{data[140]});
        res.insert_or_assign("data[141]", DumpEntry{data[141]});
        res.insert_or_assign("data[142]", DumpEntry{data[142]});
        res.insert_or_assign("data[143]", DumpEntry{data[143]});
        res.insert_or_assign("data[144]", DumpEntry{data[144]});
        res.insert_or_assign("data[145]", DumpEntry{data[145]});
        res.insert_or_assign("data[146]", DumpEntry{data[146]});
        res.insert_or_assign("data[147]", DumpEntry{data[147]});
        res.insert_or_assign("data[148]", DumpEntry{data[148]});
        res.insert_or_assign("data[149]", DumpEntry{data[149]});
        res.insert_or_assign("data[150]", DumpEntry{data[150]});
        res.insert_or_assign("data[151]", DumpEntry{data[151]});
        res.insert_or_assign("data[152]", DumpEntry{data[152]});
        res.insert_or_assign("data[153]", DumpEntry{data[153]});
        res.insert_or_assign("data[154]", DumpEntry{data[154]});
        res.insert_or_assign("data[155]", DumpEntry{data[155]});
        res.insert_or_assign("data[156]", DumpEntry{data[156]});
        res.insert_or_assign("data[157]", DumpEntry{data[157]});
        res.insert_or_assign("data[158]", DumpEntry{data[158]});
        res.insert_or_assign("data[159]", DumpEntry{data[159]});
        res.insert_or_assign("data[160]", DumpEntry{data[160]});
        res.insert_or_assign("data[161]", DumpEntry{data[161]});
        res.insert_or_assign("data[162]", DumpEntry{data[162]});
        res.insert_or_assign("data[163]", DumpEntry{data[163]});
        res.insert_or_assign("data[164]", DumpEntry{data[164]});
        res.insert_or_assign("data[165]", DumpEntry{data[165]});
        res.insert_or_assign("data[166]", DumpEntry{data[166]});
        res.insert_or_assign("data[167]", DumpEntry{data[167]});
        res.insert_or_assign("data[168]", DumpEntry{data[168]});
        res.insert_or_assign("data[169]", DumpEntry{data[169]});
        res.insert_or_assign("data[170]", DumpEntry{data[170]});
        res.insert_or_assign("data[171]", DumpEntry{data[171]});
        res.insert_or_assign("data[172]", DumpEntry{data[172]});
        res.insert_or_assign("data[173]", DumpEntry{data[173]});
        res.insert_or_assign("data[174]", DumpEntry{data[174]});
        res.insert_or_assign("data[175]", DumpEntry{data[175]});
        res.insert_or_assign("data[176]", DumpEntry{data[176]});
        res.insert_or_assign("data[177]", DumpEntry{data[177]});
        res.insert_or_assign("data[178]", DumpEntry{data[178]});
        res.insert_or_assign("data[179]", DumpEntry{data[179]});
        res.insert_or_assign("data[180]", DumpEntry{data[180]});
        res.insert_or_assign("data[181]", DumpEntry{data[181]});
        res.insert_or_assign("data[182]", DumpEntry{data[182]});
        res.insert_or_assign("data[183]", DumpEntry{data[183]});
        res.insert_or_assign("data[184]", DumpEntry{data[184]});
        res.insert_or_assign("data[185]", DumpEntry{data[185]});
        res.insert_or_assign("data[186]", DumpEntry{data[186]});
        res.insert_or_assign("data[187]", DumpEntry{data[187]});
        res.insert_or_assign("data[188]", DumpEntry{data[188]});
        res.insert_or_assign("data[189]", DumpEntry{data[189]});
        res.insert_or_assign("data[190]", DumpEntry{data[190]});
        res.insert_or_assign("data[191]", DumpEntry{data[191]});
        res.insert_or_assign("data[192]", DumpEntry{data[192]});
        res.insert_or_assign("data[193]", DumpEntry{data[193]});
        res.insert_or_assign("data[194]", DumpEntry{data[194]});
        res.insert_or_assign("data[195]", DumpEntry{data[195]});
        res.insert_or_assign("data[196]", DumpEntry{data[196]});
        res.insert_or_assign("data[197]", DumpEntry{data[197]});
        res.insert_or_assign("data[198]", DumpEntry{data[198]});
        res.insert_or_assign("data[199]", DumpEntry{data[199]});
        return res;
    }
    //! Dump the register and fields of `ipCores::RegToStream`
    //!
    //! @param regToStream A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under regToStream
    inline DumpMap dump([[maybe_unused]] const ipCores::RegToStream& regToStream)
    {
        DumpMap res{regToStream.base()};
        res.insert_or_assign("regToStream.ctrl", DumpEntry{regToStream.ctrl});
        res.insert_or_assign("regToStream.ctrl.rst", DumpEntry{regToStream.ctrl.rst});
        res.insert_or_assign("regToStream.ctrl.start", DumpEntry{regToStream.ctrl.start});
        res.insert_or_assign("regToStream.numData", DumpEntry{regToStream.numData});
        res.insert_or_assign("regToStream.tkeep", DumpEntry{regToStream.tkeep});
        return res;
    }
}
