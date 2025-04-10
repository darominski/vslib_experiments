// This header file was auto-generated using cheby
// User: daromins
// Date: 2025-04-08 15:20:44.834730
// Source map: stream_to_reg.cheby
// Command used: /home/daromins/project/code/cheby/proto/cheby.py --gen-cpp=stream_to_reg.hpp -i stream_to_reg.cheby
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

    //! ::StreamToReg
    //!
    //! Interface registers
    struct StreamToReg : MemModule<3072, uint32_t, attributes::ByteOrdering::little, attributes::WordOrdering::big>
    {
        // No version information provided.

        using MemModule::MemModule;

        //! Make this memory item point to another memory location
        //!
        //! @param new_base the new base address in memory that the memory item will map to
        void relocate(uint8_t* new_base) noexcept
        {
            // Construct new item in place using the new base address
            *this = StreamToReg(new_base);
        }

        struct Ctrl : MemReg<StreamToReg, 4, attributes::AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Ctrl, 0, 0, attributes::AccessMode::RW, bool> resetPb{
                base() + 0};   //!< reset_pb signal for the Aurora IP
            MemField<Ctrl, 1, 1, attributes::AccessMode::RW, bool> pmaInit{
                base() + 0};   //!< pma_init signal for the Aurora IP
            //! loopback config for the gt transceiver - "000" no loopback -
            //! "001" near-end pcs - "010" near-end pma - "100" far-end pcs
            //! - "110" far-end pma
            MemField<Ctrl, 4, 6, attributes::AccessMode::RW, uint8_t> loopback{base() + 0};
            //! Select which data is output over Aurora. (direct loopback -
            //! 0, from reg_to_stream IP - 1)
            MemField<Ctrl, 7, 7, attributes::AccessMode::RW, bool> selOutput{base() + 0};
        };

        struct Status : MemReg<StreamToReg, 4, attributes::AccessMode::RO, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Status, 0, 0, attributes::AccessMode::RO, bool> channelUp{base() + 0};   //!< (no comment provided)
            MemField<Status, 1, 1, attributes::AccessMode::RO, bool> gtPllLock{base() + 0};   //!< (no comment provided)
            MemField<Status, 2, 2, attributes::AccessMode::RO, bool> laneUp{base() + 0};      //!< (no comment provided)
            MemField<Status, 3, 3, attributes::AccessMode::RO, bool> pllLocked{base() + 0};   //!< (no comment provided)
            MemField<Status, 4, 4, attributes::AccessMode::RO, bool> softErr{base() + 0};     //!< (no comment provided)
            MemField<Status, 5, 5, attributes::AccessMode::RO, bool> hardErr{base() + 0};     //!< (no comment provided)
            MemField<Status, 6, 6, attributes::AccessMode::RO, bool> gtPowergood{
                base() + 0};                                                                  //!< (no comment provided)
            MemField<Status, 7, 7, attributes::AccessMode::RO, bool> gtReset{base() + 0};     //!< (no comment provided)
            MemField<Status, 8, 8, attributes::AccessMode::RO, bool> linkReset{base() + 0};   //!< (no comment provided)
            MemField<Status, 9, 9, attributes::AccessMode::RO, bool> sysReset{base() + 0};    //!< (no comment provided)
            MemField<Status, 10, 10, attributes::AccessMode::RO, bool> almostFull{
                base() + 0};   //!< The send FIFO is almost full
            //! The send FIFO has only 4 free slots. If set, this indicates
            //! a problem with the FIFO size or that data is not sent fast
            //! enough and the Aurora core generates backpressure.
            MemField<Status, 11, 11, attributes::AccessMode::RO, bool> progFull{base() + 0};
        };

        struct NumData : MemReg<StreamToReg, 4, attributes::AccessMode::RO, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct DataArrayItem : MemReg<StreamToReg, 8, attributes::AccessMode::RO, uint64_t>
        {
            using MemReg::MemReg;
        };
        using DataArray = MemArray<StreamToReg, DataArrayItem, 100, 8>;

        struct KeepArrayItem : MemReg<StreamToReg, 8, attributes::AccessMode::RO, uint64_t>
        {
            using MemReg::MemReg;
        };
        using KeepArray = MemArray<StreamToReg, KeepArrayItem, 100, 8>;

        Ctrl      ctrl{base() + 0};      //!< Control register
        Status    status{base() + 4};    //!< Status register
        NumData   numData{base() + 8};   //!< How many data words have been received.
        DataArray data{base() + 1024};   //!< Data received from Aurora
        KeepArray keep{base() + 2048};   //!< TKEEPs received from Aurora
    };
}

// Populate the `mmpp::utils` namespace with functions pertaining to `ipCores`
namespace mmpp::utils
{

    // ************************************************************

    //! Dump the register and fields of `ipCores::StreamToReg::DataArray`
    //!
    //! @param data A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under data
    inline DumpMap dump([[maybe_unused]] const ipCores::StreamToReg::DataArray& data)
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

    //! Dump the register and fields of `ipCores::StreamToReg::KeepArray`
    //!
    //! @param keep A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under keep
    inline DumpMap dump([[maybe_unused]] const ipCores::StreamToReg::KeepArray& keep)
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
        res.insert_or_assign("keep[100]", DumpEntry{keep[100]});
        res.insert_or_assign("keep[101]", DumpEntry{keep[101]});
        res.insert_or_assign("keep[102]", DumpEntry{keep[102]});
        res.insert_or_assign("keep[103]", DumpEntry{keep[103]});
        res.insert_or_assign("keep[104]", DumpEntry{keep[104]});
        res.insert_or_assign("keep[105]", DumpEntry{keep[105]});
        res.insert_or_assign("keep[106]", DumpEntry{keep[106]});
        res.insert_or_assign("keep[107]", DumpEntry{keep[107]});
        res.insert_or_assign("keep[108]", DumpEntry{keep[108]});
        res.insert_or_assign("keep[109]", DumpEntry{keep[109]});
        res.insert_or_assign("keep[110]", DumpEntry{keep[110]});
        res.insert_or_assign("keep[111]", DumpEntry{keep[111]});
        res.insert_or_assign("keep[112]", DumpEntry{keep[112]});
        res.insert_or_assign("keep[113]", DumpEntry{keep[113]});
        res.insert_or_assign("keep[114]", DumpEntry{keep[114]});
        res.insert_or_assign("keep[115]", DumpEntry{keep[115]});
        res.insert_or_assign("keep[116]", DumpEntry{keep[116]});
        res.insert_or_assign("keep[117]", DumpEntry{keep[117]});
        res.insert_or_assign("keep[118]", DumpEntry{keep[118]});
        res.insert_or_assign("keep[119]", DumpEntry{keep[119]});
        res.insert_or_assign("keep[120]", DumpEntry{keep[120]});
        res.insert_or_assign("keep[121]", DumpEntry{keep[121]});
        res.insert_or_assign("keep[122]", DumpEntry{keep[122]});
        res.insert_or_assign("keep[123]", DumpEntry{keep[123]});
        res.insert_or_assign("keep[124]", DumpEntry{keep[124]});
        res.insert_or_assign("keep[125]", DumpEntry{keep[125]});
        res.insert_or_assign("keep[126]", DumpEntry{keep[126]});
        res.insert_or_assign("keep[127]", DumpEntry{keep[127]});
        res.insert_or_assign("keep[128]", DumpEntry{keep[128]});
        res.insert_or_assign("keep[129]", DumpEntry{keep[129]});
        res.insert_or_assign("keep[130]", DumpEntry{keep[130]});
        res.insert_or_assign("keep[131]", DumpEntry{keep[131]});
        res.insert_or_assign("keep[132]", DumpEntry{keep[132]});
        res.insert_or_assign("keep[133]", DumpEntry{keep[133]});
        res.insert_or_assign("keep[134]", DumpEntry{keep[134]});
        res.insert_or_assign("keep[135]", DumpEntry{keep[135]});
        res.insert_or_assign("keep[136]", DumpEntry{keep[136]});
        res.insert_or_assign("keep[137]", DumpEntry{keep[137]});
        res.insert_or_assign("keep[138]", DumpEntry{keep[138]});
        res.insert_or_assign("keep[139]", DumpEntry{keep[139]});
        res.insert_or_assign("keep[140]", DumpEntry{keep[140]});
        res.insert_or_assign("keep[141]", DumpEntry{keep[141]});
        res.insert_or_assign("keep[142]", DumpEntry{keep[142]});
        res.insert_or_assign("keep[143]", DumpEntry{keep[143]});
        res.insert_or_assign("keep[144]", DumpEntry{keep[144]});
        res.insert_or_assign("keep[145]", DumpEntry{keep[145]});
        res.insert_or_assign("keep[146]", DumpEntry{keep[146]});
        res.insert_or_assign("keep[147]", DumpEntry{keep[147]});
        res.insert_or_assign("keep[148]", DumpEntry{keep[148]});
        res.insert_or_assign("keep[149]", DumpEntry{keep[149]});
        res.insert_or_assign("keep[150]", DumpEntry{keep[150]});
        res.insert_or_assign("keep[151]", DumpEntry{keep[151]});
        res.insert_or_assign("keep[152]", DumpEntry{keep[152]});
        res.insert_or_assign("keep[153]", DumpEntry{keep[153]});
        res.insert_or_assign("keep[154]", DumpEntry{keep[154]});
        res.insert_or_assign("keep[155]", DumpEntry{keep[155]});
        res.insert_or_assign("keep[156]", DumpEntry{keep[156]});
        res.insert_or_assign("keep[157]", DumpEntry{keep[157]});
        res.insert_or_assign("keep[158]", DumpEntry{keep[158]});
        res.insert_or_assign("keep[159]", DumpEntry{keep[159]});
        res.insert_or_assign("keep[160]", DumpEntry{keep[160]});
        res.insert_or_assign("keep[161]", DumpEntry{keep[161]});
        res.insert_or_assign("keep[162]", DumpEntry{keep[162]});
        res.insert_or_assign("keep[163]", DumpEntry{keep[163]});
        res.insert_or_assign("keep[164]", DumpEntry{keep[164]});
        res.insert_or_assign("keep[165]", DumpEntry{keep[165]});
        res.insert_or_assign("keep[166]", DumpEntry{keep[166]});
        res.insert_or_assign("keep[167]", DumpEntry{keep[167]});
        res.insert_or_assign("keep[168]", DumpEntry{keep[168]});
        res.insert_or_assign("keep[169]", DumpEntry{keep[169]});
        res.insert_or_assign("keep[170]", DumpEntry{keep[170]});
        res.insert_or_assign("keep[171]", DumpEntry{keep[171]});
        res.insert_or_assign("keep[172]", DumpEntry{keep[172]});
        res.insert_or_assign("keep[173]", DumpEntry{keep[173]});
        res.insert_or_assign("keep[174]", DumpEntry{keep[174]});
        res.insert_or_assign("keep[175]", DumpEntry{keep[175]});
        res.insert_or_assign("keep[176]", DumpEntry{keep[176]});
        res.insert_or_assign("keep[177]", DumpEntry{keep[177]});
        res.insert_or_assign("keep[178]", DumpEntry{keep[178]});
        res.insert_or_assign("keep[179]", DumpEntry{keep[179]});
        res.insert_or_assign("keep[180]", DumpEntry{keep[180]});
        res.insert_or_assign("keep[181]", DumpEntry{keep[181]});
        res.insert_or_assign("keep[182]", DumpEntry{keep[182]});
        res.insert_or_assign("keep[183]", DumpEntry{keep[183]});
        res.insert_or_assign("keep[184]", DumpEntry{keep[184]});
        res.insert_or_assign("keep[185]", DumpEntry{keep[185]});
        res.insert_or_assign("keep[186]", DumpEntry{keep[186]});
        res.insert_or_assign("keep[187]", DumpEntry{keep[187]});
        res.insert_or_assign("keep[188]", DumpEntry{keep[188]});
        res.insert_or_assign("keep[189]", DumpEntry{keep[189]});
        res.insert_or_assign("keep[190]", DumpEntry{keep[190]});
        res.insert_or_assign("keep[191]", DumpEntry{keep[191]});
        res.insert_or_assign("keep[192]", DumpEntry{keep[192]});
        res.insert_or_assign("keep[193]", DumpEntry{keep[193]});
        res.insert_or_assign("keep[194]", DumpEntry{keep[194]});
        res.insert_or_assign("keep[195]", DumpEntry{keep[195]});
        res.insert_or_assign("keep[196]", DumpEntry{keep[196]});
        res.insert_or_assign("keep[197]", DumpEntry{keep[197]});
        res.insert_or_assign("keep[198]", DumpEntry{keep[198]});
        res.insert_or_assign("keep[199]", DumpEntry{keep[199]});
        return res;
    }
    //! Dump the register and fields of `ipCores::StreamToReg`
    //!
    //! @param streamToReg A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under streamToReg
    inline DumpMap dump([[maybe_unused]] const ipCores::StreamToReg& streamToReg)
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

        return res;
    }
}
