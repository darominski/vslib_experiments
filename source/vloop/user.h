#pragma once

#include <fmt/format.h>
#include <string>
#include <unistd.h>

#include "afe.h"
#include "afe_rst.h"
#include "afe_vdc_bal.h"
#include "cheby_gen/reg_to_stream_cpp.h"
#include "cheby_gen/stream_to_reg_cpp.h"
#include "peripherals/reg_to_stream.h"
#include "peripherals/stream_to_reg.h"
// #include "pops_current_balancing.h"
// #include "pops_current_balancing_old.h"
// #include "pops_dispatcher.h"
#include "vslib.h"

namespace user
{
    class Converter : public vslib::IConverter
    {
      public:
        Converter(vslib::RootComponent& root) noexcept
            : vslib::IConverter("example", root),
              interrupt_1("aurora", *this, 121, vslib::InterruptPriority::high, RTTask),
              //   afe_vdc_bal("afe_rst", *this),
              pll("pll", *this),
              abc_to_dq0_v("abc_to_dq0_voltage", *this, 100'000),
              abc_to_dq0_i("abc_to_dq0_current", *this, 100'000),
              dq0_to_abc("dq0_to_abc", *this, 100'000),
              power_3ph_instant("power_3ph_instant", *this),
              rst_outer_vdc("rst_outer_vdc", *this),
              rst_outer_id("rst_outer_id", *this),
              rst_outer_iq("rst_outer_iq", *this),
              rst_inner_vd("rst_inner_vd", *this),
              rst_inner_vq("rst_inner_vq", *this),
              limit("limit", *this),
              rst_vdc("rst_vdc", *this),
              iir_vdc("iir_vdc", *this),
              inductance(*this, "inductance"),
              frequency(*this, "frequency"),
              v_base(*this, "v_base"),
              i_base(*this, "i_base"),
              m_s2rcpp(reinterpret_cast<uint8_t*>(0xA0200000)),
              m_r2scpp(reinterpret_cast<uint8_t*>(0xA0100000))
        {
            // initialize all your objects that need initializing
        }

        // Define your public Components here
        vslib::PeripheralInterrupt<Converter> interrupt_1;
        // ...
        // end of your Components

        // Define your Parameters here

        // end of your Parameters

        void init() override
        {
            m_s2rcpp.ctrl.pmaInit.set(false);
            sleep(2);

            m_s2rcpp.ctrl.resetPb.set(false);
            sleep(1);

            m_s2rcpp.ctrl.selOutput.set(true);

            if (!(m_s2rcpp.status.channelUp.get() && m_s2rcpp.status.gtPllLock.get() && m_s2rcpp.status.laneUp.get()
                  && m_s2rcpp.status.pllLocked.get() && m_s2rcpp.status.gtPowergood.get()))
            {
                printf("Unexpected status: 0x%#08x\n", m_s2rcpp.ctrl.read());
            }

            if (m_s2rcpp.status.linkReset.get() || m_s2rcpp.status.sysReset.get())
            {
                printf("Link is in reset\n");
            }

            if (m_s2rcpp.status.softErr.get() || m_s2rcpp.status.hardErr.get())
            {
                printf("Got an error\n");
            }

            // kria transfer rate: 100us
            m_r2scpp.numData.write(num_data * 2);
            m_r2scpp.tkeep.write(0x0000FFFF);

            printf("Link up and good. Ready to receive data.\n");
            interrupt_1.start();
        }

        constexpr static int n_elements = 100'000;

        void backgroundTask() override
        {
#ifdef PERFORMANCE_TESTS
            if (counter > n_elements)
            {
                interrupt_1.stop();
                const double scaling = 1.0 / 1.3333;   // 1 / 1.3333 GHz
                double const mean    = interrupt_1.average() * scaling;
                std::cout << "Average time per interrupt: (" << mean << " +- "
                          << interrupt_1.standardDeviation(interrupt_1.average()) * scaling << ") ns" << std::endl;
                auto const histogram = interrupt_1.histogramMeasurements<100>(interrupt_1.min(), interrupt_1.max());
                for (auto const& value : histogram.getData())
                {
                    std::cout << value << " ";
                }
                std::cout << std::endl;
                const auto bin_with_max = histogram.getBinWithMax();
                const auto edges        = histogram.getBinEdges(bin_with_max);
                std::cout << "bin with max: " << bin_with_max
                          << ", centered at: " << 0.5 * (edges.first * scaling + edges.second * scaling) << std::endl;
                const auto min = interrupt_1.min() * scaling;
                const auto max = interrupt_1.max() * scaling;
                std::cout << "min: " << min << " ns, max: " << max << " ns" << std::endl;
                exit(0);
            }
#endif
        }

        template<typename SourceType, typename TargetType>
        static TargetType cast(SourceType input)
        {
            return std::bit_cast<TargetType>(input);
        }

        static void RTTask(Converter& converter)
        {
            // collect inputs
            for (uint32_t index = 0; index < num_data; index++)
            {
                converter.m_data[index] = cast<uint64_t, double>(converter.m_s2rcpp.data[index].read());
            }

            const double regulation_on = converter.m_data[0];
            const double v_dc_ref      = converter.m_data[1];
            const double v_dc_p        = converter.m_data[2];
            const double v_dc_n        = converter.m_data[3];
            const double q_ref         = converter.m_data[4];
            const double v_a           = converter.m_data[5];
            const double v_b           = converter.m_data[6];
            const double v_c           = converter.m_data[7];
            const double i_a           = converter.m_data[8];
            const double i_b           = converter.m_data[9];
            const double i_c           = converter.m_data[10];
            // const double iq_ref        = converter.m_data[11];
            // const double id_ref        = converter.m_data[12];

            const double v_dc_meas = v_dc_p + v_dc_n;
            const double v_dc_diff = v_dc_p - v_dc_n;

            //
            // Synchronisation, measurement, and change of reference frame
            //
            const auto wt_pll = converter.pll.synchronise(
                regulation_on * v_a * converter.m_si_to_pu, regulation_on * v_b * converter.m_si_to_pu,
                regulation_on * v_c * converter.m_si_to_pu
            );
            const auto [vd_meas, vq_meas, zero_v] = converter.abc_to_dq0_v.transform(
                regulation_on * v_a * converter.m_si_to_pu, regulation_on * v_b * converter.m_si_to_pu,
                regulation_on * v_c * converter.m_si_to_pu, wt_pll
            );
            const auto [id_meas, iq_meas, zero_i] = converter.abc_to_dq0_i.transform(
                regulation_on * i_a * converter.m_i_to_pu, regulation_on * i_b * converter.m_i_to_pu,
                regulation_on * i_c * converter.m_i_to_pu, wt_pll
            );
            const auto [p_meas, q_meas] = converter.power_3ph_instant.calculate(
                regulation_on * v_a, regulation_on * v_b, regulation_on * v_c, regulation_on * i_a, regulation_on * i_b,
                regulation_on * i_c
            );

            double p_ref = 0;
            if (regulation_on > 0)
            {
                // needs to not run until regulation is set to ON
                //
                // Outer loop: Vdc control
                //
                p_ref = converter.rst_outer_vdc.control(
                    regulation_on * pow(v_dc_ref, 2), regulation_on * pow(v_dc_meas, 2)
                );
            }

            //
            // Outer loop: power regulation
            // 2 RSTs
            const auto id_ref
                = converter.rst_outer_id.control(regulation_on * p_ref * converter.m_va_to_pu, regulation_on * p_meas);
            const auto iq_ref = -converter.rst_outer_iq.control(regulation_on * q_ref, regulation_on * q_meas);

            //
            // Inner loop: dq-vector current control
            //
            // RST + 2 * ff for each loop
            const auto vd_ref = converter.rst_inner_vd.control(-regulation_on * id_ref, regulation_on * id_meas)
                                + vd_meas
                                + regulation_on * iq_meas * converter.i_base * converter.m_wl * converter.m_si_to_pu;
            const auto vq_ref = converter.rst_inner_vq.control(-regulation_on * iq_ref, regulation_on * iq_meas)
                                + vq_meas
                                - regulation_on * id_meas * converter.i_base * converter.m_wl * converter.m_si_to_pu;

            //
            // Frame conversion
            //
            const auto vd_ref_lim = converter.limit.limit(vd_ref);
            const auto vq_ref_lim = converter.limit.limit(vq_ref);

            const auto [v_a_ref, v_b_ref, v_c_ref]
                = converter.dq0_to_abc.transform(vd_ref_lim, vq_ref_lim, 0.0, wt_pll);

            // const auto [v_a_ref, v_b_ref, v_c_ref] = converter.afe_vdc_bal.vdc_control(
            // v_a, v_b, v_c, i_a, i_b, i_c, v_dc_ref, v_dc_meas, q_ref, regulation_on
            // );

            const auto v_dc_diff_filtered = converter.iir_vdc.filter(regulation_on * v_dc_diff);
            const auto m0                 = converter.rst_vdc.control(0.0, regulation_on * v_dc_diff_filtered);

            converter.m_data[0]  = v_a_ref;
            converter.m_data[1]  = v_b_ref;
            converter.m_data[2]  = v_c_ref;
            converter.m_data[3]  = m0;
            converter.m_data[4]  = v_dc_diff;
            converter.m_data[5]  = v_dc_diff_filtered;
            converter.m_data[6]  = vd_ref;
            converter.m_data[7]  = vq_ref;
            converter.m_data[8]  = p_ref * converter.m_va_to_pu;
            converter.m_data[9]  = iq_meas;
            converter.m_data[10] = iq_ref;
            converter.m_data[11] = id_meas;
            converter.m_data[12] = v_dc_ref;
            converter.m_data[13] = v_dc_meas;
            converter.m_data[14] = p_meas;
            converter.m_data[15] = wt_pll;
            converter.m_data[16] = vq_meas;
            converter.m_data[17] = id_ref;
            converter.m_data[18] = q_meas;

            // write to output registers
            for (uint32_t index = 0; index < num_data; index++)
            {
                // converter.m_r2scpp.data[index].write(cast<double, uint64_t>(converter.m_data[index]));
                converter.m_r2scpp.data[index].write(cast<double, uint64_t>(converter.m_data[index]));
            }

            // send it away
            // trigger connection
            converter.m_r2scpp.ctrl.start.set(true);
            converter.counter++;
        }

        // static void RTTaskPerf(Converter& converter)
        // {
        //     // for (int index=0; index<50; index++)
        //     // {
        //         asm volatile("isb; dsb sy");
        //         const volatile double meas = 0.5;//static_cast<double>((std::rand() / RAND_MAX - 1) * 100.0);
        //         const volatile double ref = 1.0;//static_cast<double>((std::rand() / RAND_MAX - 1) * 100.0);

        //         converter.m_data[0] = meas;
        //         converter.m_data[1] = ref;

        //         const volatile double act = 0.0;
        //     //     const volatile double act = converter.rst.control(ref, meas);

        //         converter.m_data[2] = act;
        //         asm volatile("isb; dsb sy");
        //     // }
        //     converter.counter++;
        // }

        vslib::SRFPLL                       pll;
        vslib::AbcToDq0Transform            abc_to_dq0_v;
        vslib::AbcToDq0Transform            abc_to_dq0_i;
        vslib::Dq0ToAbcTransform            dq0_to_abc;
        vslib::InstantaneousPowerThreePhase power_3ph_instant;
        vslib::RST<1>                       rst_outer_vdc;
        vslib::RST<2>                       rst_outer_id;
        vslib::RST<2>                       rst_outer_iq;
        vslib::RST<1>                       rst_inner_vd;
        vslib::RST<1>                       rst_inner_vq;
        vslib::LimitRange<double>           limit;

        vslib::RST<1>       rst_vdc;
        vslib::IIRFilter<2> iir_vdc;

        // Owned Parameters
        vslib::Parameter<double> inductance;   //!< Inductance of the system [H]
        vslib::Parameter<double> frequency;    //!< Current frequency [Hz]
        vslib::Parameter<double> v_base;       //!< Base voltage [V]
        vslib::Parameter<double> i_base;       //!< Base current [A]

        std::optional<fgc4::utils::Warning> verifyParameters() override
        {
            m_wl = 2.0 * std::numbers::pi * frequency.toValidate() * inductance.toValidate();

            // conversion constants, based on base voltage and base current:
            m_si_to_pu = sqrt(3.0 / 2.0) / v_base.toValidate();
            m_i_to_pu  = 1.0 / i_base.toValidate();
            m_va_to_pu = sqrt(2.0 / 3.0) * m_i_to_pu / v_base.toValidate();

            m_pu_to_v = 1.0 / m_si_to_pu;

            return {};
        }

      private:
        int counter{0};

        constexpr static uint32_t    num_data{20};
        std::array<double, num_data> m_data;

        myModule::StreamToReg m_s2rcpp;
        myModule::RegToStream m_r2scpp;

        double m_wl{0.0};
        double m_si_to_pu{0.0};
        double m_pu_to_v{0.0};
        double m_i_to_pu{0.0};
        double m_va_to_pu{0.0};
    };

}   // namespace user
