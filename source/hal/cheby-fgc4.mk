# FGC4_HOME make argument must point to a checkout of https://gitlab.cern.ch/cce/fgc4.git

VHD = $(FGC4_HOME)/submodules/cce/common/vhd

ALL_SRC = \
	$(VHD)/generic/counters/src/loadable_counter_regs.cheby \
	$(VHD)/interfaces/adc_serial/adc_serial_regs.cheby \
	$(VHD)/interfaces/dac_serial/dac_serial_regs.cheby \
	$(VHD)/interfaces/in_out/in_out_regs.cheby

OUTPREFIX=inc/cheby_gen/

all: \
	$(OUTPREFIX)pb_top.html \
	$(OUTPREFIX)adc_serial_regs.h \
	$(OUTPREFIX)dac_serial_regs.h \
	$(OUTPREFIX)in_out_regs.h \
	$(OUTPREFIX)loadable_counter_regs.h \
	$(OUTPREFIX)pb_top.h \

$(OUTPREFIX)pb_top.html: $(FGC4_HOME)/pb/pb_top.cheby $(ALL_SRC)
	cheby --gen-doc -i $< > $@

$(OUTPREFIX)pb_top.h: $(ALL_SRC)
	cheby --gen-c -i $(FGC4_HOME)/pb/pb_top.cheby > $@

$(OUTPREFIX)%.h: $(VHD)/generic/counters/src/%.cheby
	cheby --gen-c -i $< > $@

$(OUTPREFIX)%.h: $(VHD)/interfaces/adc_serial/%.cheby
	cheby --gen-c -i $< > $@

$(OUTPREFIX)%.h: $(VHD)/interfaces/dac_serial/%.cheby
	cheby --gen-c -i $< > $@

$(OUTPREFIX)%.h: $(VHD)/interfaces/in_out/%.cheby
	cheby --gen-c -i $< > $@
