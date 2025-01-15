// This header file was auto-generated using cheby
// User: daromins
// Date: 2025-01-15 13:53:15.739695
// Source map: pwm_regs_axi.cheby
// Command used: /home/daromins/project/misc/cheby/proto/cheby.py --gen-cpp=pwm_regs_axi.h -i pwm_regs_axi.cheby
// ************************************************************

#pragma once

#include <mem_primitives.h>

namespace myModule
{

    using namespace mmpp;
    using mmpp::utils::DumpEntry;
    using mmpp::utils::DumpMap;

    //! ::PwmRegsAxi
    //!
    //! PWM IP with AXI addressable memory map wrapper
    struct PwmRegsAxi : MemModule<uint32_t, ByteOrdering::big, WordOrdering::little>
    {
        using MemModule::MemModule;
        [[nodiscard]] static constexpr size_t csize() noexcept
        {
            return 64;
        }
        [[nodiscard]] size_t size() const noexcept override
        {
            return 64;
        }

        //! Make this memory item point to another memory location
        //!
        //! @param new_base the new base address in memory that the memory item will map to
        void relocate(uint8_t* new_base) noexcept
        {
            // Construct new item in place using the new base address
            *this = PwmRegsAxi(new_base);
        }

        struct Regs : MemSubmodule<PwmRegsAxi>
        {
            using MemSubmodule::MemSubmodule;
            [[nodiscard]] static constexpr size_t csize() noexcept
            {
                return 64;
            }
            [[nodiscard]] size_t size() const noexcept override
            {
                return 64;
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

            struct Ctrl : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;

                MemField<Ctrl, 0, 0, AccessMode::RW, bool> killA{base() + 0};   //!< (no comment provided)
                MemField<Ctrl, 1, 1, AccessMode::RW, bool> killB{base() + 0};   //!< (no comment provided)
                MemField<Ctrl, 2, 2, AccessMode::RW, bool> en{base() + 0};      //!< (no comment provided)
                MemField<Ctrl, 3, 3, AccessMode::RW, bool> reset{base() + 0};   //!< (no comment provided)
            };

            struct CtrBits : MemReg<Regs, AccessMode::RO, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct DeadtimeBits : MemReg<Regs, AccessMode::RO, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct Conf : MemReg<Regs, AccessMode::RW, uint8_t>
            {
                using MemReg::MemReg;

                MemField<Conf, 0, 1, AccessMode::RW, LoadMode> loadMode{base() + 0};       //!< (no comment provided)
                MemField<Conf, 2, 2, AccessMode::RW, bool>     killPolA{base() + 0};       //!< (no comment provided)
                MemField<Conf, 3, 3, AccessMode::RW, bool>     killPolB{base() + 0};       //!< (no comment provided)
                MemField<Conf, 4, 4, AccessMode::RW, bool>     enPwmCheck{base() + 0};     //!< (no comment provided)
                MemField<Conf, 5, 5, AccessMode::RW, bool>     enStCheck{base() + 0};      //!< (no comment provided)
                MemField<Conf, 6, 6, AccessMode::RW, bool>     enValueCheck{base() + 0};   //!< (no comment provided)
            };

            struct Cc : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct Ctrh : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct Dtctrl : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;

                MemField<Dtctrl, 0, 0, AccessMode::RW, bool> bpdt{base() + 0};       //!< (no comment provided)
                MemField<Dtctrl, 1, 1, AccessMode::RW, bool> disableA{base() + 0};   //!< (no comment provided)
                MemField<Dtctrl, 2, 2, AccessMode::RW, bool> disableB{base() + 0};   //!< (no comment provided)
                MemField<Dtctrl, 3, 3, AccessMode::RW, bool> inva{base() + 0};       //!< (no comment provided)
                MemField<Dtctrl, 4, 4, AccessMode::RW, bool> invb{base() + 0};       //!< (no comment provided)
            };

            struct Deadtime : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct ExtDeadtime : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct MinSwitchTimeSc : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct MinModIdx : MemReg<Regs, AccessMode::RW, uint32_t>
            {
                using MemReg::MemReg;
            };

            struct MaxModIdx : MemReg<Regs, AccessMode::RW, uint32_t>
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

        Regs regs{base() + 0};   //!< (no comment provided)
    };
}

// Populate the `dump_utils` namespace with functions pertaining to `myModule`
namespace mmpp::utils
{
    //! Specialization of `to_string` for `myModule::PwmRegsAxi::Regs::LoadMode`
    template<>
    inline std::string to_string(const myModule::PwmRegsAxi::Regs::LoadMode& val)
    {
        switch (val)
        {
            case myModule::PwmRegsAxi::Regs::LoadMode::zero:
                return "zero";
            case myModule::PwmRegsAxi::Regs::LoadMode::prd:
                return "prd";
            case myModule::PwmRegsAxi::Regs::LoadMode::zeroPrd:
                return "zeroPrd";
            case myModule::PwmRegsAxi::Regs::LoadMode::immediate:
                return "immediate";
            default:
                return "<undefined> (raw value: " + to_string(utils::as_unsigned(val)) + ")";
        }
    }

    // ************************************************************

    //! Dump the register and fields of `myModule::PwmRegsAxi::Regs`
    //!
    //! @param regs A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under regs
    inline DumpMap dump([[maybe_unused]] const myModule::PwmRegsAxi::Regs& regs)
    {
        DumpMap res{regs.base()};
        res.insert_or_assign("regs.ctrl", DumpEntry{regs.ctrl});
        res.insert_or_assign("regs.ctrl.killA", DumpEntry{regs.ctrl.killA});
        res.insert_or_assign("regs.ctrl.killB", DumpEntry{regs.ctrl.killB});
        res.insert_or_assign("regs.ctrl.en", DumpEntry{regs.ctrl.en});
        res.insert_or_assign("regs.ctrl.reset", DumpEntry{regs.ctrl.reset});
        res.insert_or_assign("regs.ctrBits", DumpEntry{regs.ctrBits});
        res.insert_or_assign("regs.deadtimeBits", DumpEntry{regs.deadtimeBits});
        res.insert_or_assign("regs.conf", DumpEntry{regs.conf});
        res.insert_or_assign("regs.conf.loadMode", DumpEntry{regs.conf.loadMode});
        res.insert_or_assign("regs.conf.killPolA", DumpEntry{regs.conf.killPolA});
        res.insert_or_assign("regs.conf.killPolB", DumpEntry{regs.conf.killPolB});
        res.insert_or_assign("regs.conf.enPwmCheck", DumpEntry{regs.conf.enPwmCheck});
        res.insert_or_assign("regs.conf.enStCheck", DumpEntry{regs.conf.enStCheck});
        res.insert_or_assign("regs.conf.enValueCheck", DumpEntry{regs.conf.enValueCheck});
        res.insert_or_assign("regs.cc", DumpEntry{regs.cc});
        res.insert_or_assign("regs.ctrh", DumpEntry{regs.ctrh});
        res.insert_or_assign("regs.dtctrl", DumpEntry{regs.dtctrl});
        res.insert_or_assign("regs.dtctrl.bpdt", DumpEntry{regs.dtctrl.bpdt});
        res.insert_or_assign("regs.dtctrl.disableA", DumpEntry{regs.dtctrl.disableA});
        res.insert_or_assign("regs.dtctrl.disableB", DumpEntry{regs.dtctrl.disableB});
        res.insert_or_assign("regs.dtctrl.inva", DumpEntry{regs.dtctrl.inva});
        res.insert_or_assign("regs.dtctrl.invb", DumpEntry{regs.dtctrl.invb});
        res.insert_or_assign("regs.deadtime", DumpEntry{regs.deadtime});
        res.insert_or_assign("regs.extDeadtime", DumpEntry{regs.extDeadtime});
        res.insert_or_assign("regs.minSwitchTimeSc", DumpEntry{regs.minSwitchTimeSc});
        res.insert_or_assign("regs.minModIdx", DumpEntry{regs.minModIdx});
        res.insert_or_assign("regs.maxModIdx", DumpEntry{regs.maxModIdx});
        return res;
    }
    //! Dump the register and fields of `myModule::PwmRegsAxi`
    //!
    //! @param pwmRegsAxi A reference to the module
    //! @returns A `dump_utils::DumpMap` with all the register and fields under pwmRegsAxi
    inline DumpMap dump([[maybe_unused]] const myModule::PwmRegsAxi& pwmRegsAxi)
    {
        DumpMap res{pwmRegsAxi.base()};
        res.insert_or_assign("pwmRegsAxi.regs.ctrl", DumpEntry{pwmRegsAxi.regs.ctrl});
        res.insert_or_assign("pwmRegsAxi.regs.ctrl.killA", DumpEntry{pwmRegsAxi.regs.ctrl.killA});
        res.insert_or_assign("pwmRegsAxi.regs.ctrl.killB", DumpEntry{pwmRegsAxi.regs.ctrl.killB});
        res.insert_or_assign("pwmRegsAxi.regs.ctrl.en", DumpEntry{pwmRegsAxi.regs.ctrl.en});
        res.insert_or_assign("pwmRegsAxi.regs.ctrl.reset", DumpEntry{pwmRegsAxi.regs.ctrl.reset});
        res.insert_or_assign("pwmRegsAxi.regs.ctrBits", DumpEntry{pwmRegsAxi.regs.ctrBits});
        res.insert_or_assign("pwmRegsAxi.regs.deadtimeBits", DumpEntry{pwmRegsAxi.regs.deadtimeBits});
        res.insert_or_assign("pwmRegsAxi.regs.conf", DumpEntry{pwmRegsAxi.regs.conf});
        res.insert_or_assign("pwmRegsAxi.regs.conf.loadMode", DumpEntry{pwmRegsAxi.regs.conf.loadMode});
        res.insert_or_assign("pwmRegsAxi.regs.conf.killPolA", DumpEntry{pwmRegsAxi.regs.conf.killPolA});
        res.insert_or_assign("pwmRegsAxi.regs.conf.killPolB", DumpEntry{pwmRegsAxi.regs.conf.killPolB});
        res.insert_or_assign("pwmRegsAxi.regs.conf.enPwmCheck", DumpEntry{pwmRegsAxi.regs.conf.enPwmCheck});
        res.insert_or_assign("pwmRegsAxi.regs.conf.enStCheck", DumpEntry{pwmRegsAxi.regs.conf.enStCheck});
        res.insert_or_assign("pwmRegsAxi.regs.conf.enValueCheck", DumpEntry{pwmRegsAxi.regs.conf.enValueCheck});
        res.insert_or_assign("pwmRegsAxi.regs.cc", DumpEntry{pwmRegsAxi.regs.cc});
        res.insert_or_assign("pwmRegsAxi.regs.ctrh", DumpEntry{pwmRegsAxi.regs.ctrh});
        res.insert_or_assign("pwmRegsAxi.regs.dtctrl", DumpEntry{pwmRegsAxi.regs.dtctrl});
        res.insert_or_assign("pwmRegsAxi.regs.dtctrl.bpdt", DumpEntry{pwmRegsAxi.regs.dtctrl.bpdt});
        res.insert_or_assign("pwmRegsAxi.regs.dtctrl.disableA", DumpEntry{pwmRegsAxi.regs.dtctrl.disableA});
        res.insert_or_assign("pwmRegsAxi.regs.dtctrl.disableB", DumpEntry{pwmRegsAxi.regs.dtctrl.disableB});
        res.insert_or_assign("pwmRegsAxi.regs.dtctrl.inva", DumpEntry{pwmRegsAxi.regs.dtctrl.inva});
        res.insert_or_assign("pwmRegsAxi.regs.dtctrl.invb", DumpEntry{pwmRegsAxi.regs.dtctrl.invb});
        res.insert_or_assign("pwmRegsAxi.regs.deadtime", DumpEntry{pwmRegsAxi.regs.deadtime});
        res.insert_or_assign("pwmRegsAxi.regs.extDeadtime", DumpEntry{pwmRegsAxi.regs.extDeadtime});
        res.insert_or_assign("pwmRegsAxi.regs.minSwitchTimeSc", DumpEntry{pwmRegsAxi.regs.minSwitchTimeSc});
        res.insert_or_assign("pwmRegsAxi.regs.minModIdx", DumpEntry{pwmRegsAxi.regs.minModIdx});
        res.insert_or_assign("pwmRegsAxi.regs.maxModIdx", DumpEntry{pwmRegsAxi.regs.maxModIdx});
        return res;
    }
}
