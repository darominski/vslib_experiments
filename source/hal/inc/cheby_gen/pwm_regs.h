// This header file was auto-generated using cheby
// User: daromins
// Date: 2025-01-15 13:53:03.425409
// Source map: pwm_regs.cheby
// Command used: /home/daromins/project/misc/cheby/proto/cheby.py --gen-cpp=pwm_regs.h -i pwm_regs.cheby
// ************************************************************

#pragma once

#include <mmpp.h>

namespace myModule
{

    using namespace mmpp;
    using mmpp::utils::DumpEntry;
    using mmpp::utils::DumpMap;

    //! ::PwmRegs
    //!
    //! pwm memory map - wishbone
    struct PwmRegs : MemModule<uint32_t, ByteOrdering::big, WordOrdering::little>
    {
        using MemModule::MemModule;
        [[nodiscard]] static constexpr size_t csize() noexcept
        {
            return 48;
        }
        [[nodiscard]] size_t size() const noexcept override
        {
            return 48;
        }

        //! Make this memory item point to another memory location
        //!
        //! @param new_base the new base address in memory that the memory item will map to
        void relocate(uint8_t* new_base) noexcept
        {
            // Construct new item in place using the new base address
            *this = PwmRegs(new_base);
        }

        // ************************************************************

        enum class LoadMode : uint8_t
        {
            zero      = 0,   //!< (no comment provided)
            prd       = 1,   //!< (no comment provided)
            zeroPrd   = 2,   //!< (no comment provided)
            immediate = 3,   //!< (no comment provided)
        };

        // ************************************************************

        struct Ctrl : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Ctrl, 0, 0, AccessMode::RW, bool> killA{base() + 0};   //!< (no comment provided)
            MemField<Ctrl, 1, 1, AccessMode::RW, bool> killB{base() + 0};   //!< (no comment provided)
            MemField<Ctrl, 2, 2, AccessMode::RW, bool> en{base() + 0};      //!< (no comment provided)
            MemField<Ctrl, 3, 3, AccessMode::RW, bool> reset{base() + 0};   //!< (no comment provided)
        };

        struct CtrBits : MemReg<PwmRegs, AccessMode::RO, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct DeadtimeBits : MemReg<PwmRegs, AccessMode::RO, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct Conf : MemReg<PwmRegs, AccessMode::RW, uint8_t>
        {
            using MemReg::MemReg;

            MemField<Conf, 0, 1, AccessMode::RW, LoadMode> loadMode{base() + 0};       //!< (no comment provided)
            MemField<Conf, 2, 2, AccessMode::RW, bool>     killPolA{base() + 0};       //!< (no comment provided)
            MemField<Conf, 3, 3, AccessMode::RW, bool>     killPolB{base() + 0};       //!< (no comment provided)
            MemField<Conf, 4, 4, AccessMode::RW, bool>     enPwmCheck{base() + 0};     //!< (no comment provided)
            MemField<Conf, 5, 5, AccessMode::RW, bool>     enStCheck{base() + 0};      //!< (no comment provided)
            MemField<Conf, 6, 6, AccessMode::RW, bool>     enValueCheck{base() + 0};   //!< (no comment provided)
        };

        struct Cc : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct Ctrh : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct Dtctrl : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;

            MemField<Dtctrl, 0, 0, AccessMode::RW, bool> bpdt{base() + 0};       //!< (no comment provided)
            MemField<Dtctrl, 1, 1, AccessMode::RW, bool> disableA{base() + 0};   //!< (no comment provided)
            MemField<Dtctrl, 2, 2, AccessMode::RW, bool> disableB{base() + 0};   //!< (no comment provided)
            MemField<Dtctrl, 3, 3, AccessMode::RW, bool> inva{base() + 0};       //!< (no comment provided)
            MemField<Dtctrl, 4, 4, AccessMode::RW, bool> invb{base() + 0};       //!< (no comment provided)
        };

        struct Deadtime : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct ExtDeadtime : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct MinSwitchTimeSc : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct MinModIdx : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        struct MaxModIdx : MemReg<PwmRegs, AccessMode::RW, uint32_t>
        {
            using MemReg::MemReg;
        };

        Ctrl            ctrl{base() + 0};               //!< (no comment provided)
        CtrBits         ctrBits{base() + 4};            //!< (no comment provided)
        DeadtimeBits    deadtimeBits{base() + 8};       //!< (no comment provided)
        Conf            conf{base() + 12};              //!< Control config
        Cc              cc{base() + 16};                //!< (no comment provided)
        Ctrh            ctrh{base() + 20};              //!< (no comment provided)
        Dtctrl          dtctrl{base() + 24};            //!< (no comment provided)
        Deadtime        deadtime{base() + 28};          //!< (no comment provided)
        ExtDeadtime     extDeadtime{base() + 32};       //!< (no comment provided)
        MinSwitchTimeSc minSwitchTimeSc{base() + 36};   //!< (no comment provided)
        MinModIdx       minModIdx{base() + 40};         //!< (no comment provided)
        MaxModIdx       maxModIdx{base() + 44};         //!< (no comment provided)
    };
}

// Populate the `dump_utils` namespace with functions pertaining to `myModule`
namespace mmpp::utils
{
    //! Specialization of `to_string` for `myModule::PwmRegs::LoadMode`
    template<>
    inline std::string to_string(const myModule::PwmRegs::LoadMode& val)
    {
        switch (val)
        {
            case myModule::PwmRegs::LoadMode::zero:
                return "zero";
            case myModule::PwmRegs::LoadMode::prd:
                return "prd";
            case myModule::PwmRegs::LoadMode::zeroPrd:
                return "zeroPrd";
            case myModule::PwmRegs::LoadMode::immediate:
                return "immediate";
            default:
                return "<undefined> (raw value: " + to_string(utils::as_unsigned(val)) + ")";
        }
    }

    //! Dump the register and fields of `myModule::PwmRegs`
    //!
    //! @param pwmRegs A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under pwmRegs
    inline DumpMap dump([[maybe_unused]] const myModule::PwmRegs& pwmRegs)
    {
        DumpMap res{pwmRegs.base()};
        res.insert_or_assign("pwmRegs.ctrl", DumpEntry{pwmRegs.ctrl});
        res.insert_or_assign("pwmRegs.ctrl.killA", DumpEntry{pwmRegs.ctrl.killA});
        res.insert_or_assign("pwmRegs.ctrl.killB", DumpEntry{pwmRegs.ctrl.killB});
        res.insert_or_assign("pwmRegs.ctrl.en", DumpEntry{pwmRegs.ctrl.en});
        res.insert_or_assign("pwmRegs.ctrl.reset", DumpEntry{pwmRegs.ctrl.reset});
        res.insert_or_assign("pwmRegs.ctrBits", DumpEntry{pwmRegs.ctrBits});
        res.insert_or_assign("pwmRegs.deadtimeBits", DumpEntry{pwmRegs.deadtimeBits});
        res.insert_or_assign("pwmRegs.conf", DumpEntry{pwmRegs.conf});
        res.insert_or_assign("pwmRegs.conf.loadMode", DumpEntry{pwmRegs.conf.loadMode});
        res.insert_or_assign("pwmRegs.conf.killPolA", DumpEntry{pwmRegs.conf.killPolA});
        res.insert_or_assign("pwmRegs.conf.killPolB", DumpEntry{pwmRegs.conf.killPolB});
        res.insert_or_assign("pwmRegs.conf.enPwmCheck", DumpEntry{pwmRegs.conf.enPwmCheck});
        res.insert_or_assign("pwmRegs.conf.enStCheck", DumpEntry{pwmRegs.conf.enStCheck});
        res.insert_or_assign("pwmRegs.conf.enValueCheck", DumpEntry{pwmRegs.conf.enValueCheck});
        res.insert_or_assign("pwmRegs.cc", DumpEntry{pwmRegs.cc});
        res.insert_or_assign("pwmRegs.ctrh", DumpEntry{pwmRegs.ctrh});
        res.insert_or_assign("pwmRegs.dtctrl", DumpEntry{pwmRegs.dtctrl});
        res.insert_or_assign("pwmRegs.dtctrl.bpdt", DumpEntry{pwmRegs.dtctrl.bpdt});
        res.insert_or_assign("pwmRegs.dtctrl.disableA", DumpEntry{pwmRegs.dtctrl.disableA});
        res.insert_or_assign("pwmRegs.dtctrl.disableB", DumpEntry{pwmRegs.dtctrl.disableB});
        res.insert_or_assign("pwmRegs.dtctrl.inva", DumpEntry{pwmRegs.dtctrl.inva});
        res.insert_or_assign("pwmRegs.dtctrl.invb", DumpEntry{pwmRegs.dtctrl.invb});
        res.insert_or_assign("pwmRegs.deadtime", DumpEntry{pwmRegs.deadtime});
        res.insert_or_assign("pwmRegs.extDeadtime", DumpEntry{pwmRegs.extDeadtime});
        res.insert_or_assign("pwmRegs.minSwitchTimeSc", DumpEntry{pwmRegs.minSwitchTimeSc});
        res.insert_or_assign("pwmRegs.minModIdx", DumpEntry{pwmRegs.minModIdx});
        res.insert_or_assign("pwmRegs.maxModIdx", DumpEntry{pwmRegs.maxModIdx});
        return res;
    }
}
