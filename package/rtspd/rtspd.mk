################################################################################
#
# rtspd
#
################################################################################

RTSPD_LICENSE = GPLv2+

define RTSPD_BUILD_CMDS
	cp -a package/rtspd/* $(@D)/
	cd $(@D) && $(TARGET_CXX) $(TARGET_CXXFLAGS) $(TARGET_LDFLAGS) \
		-I./include/ \
		./lib/libmpi.a ./lib/libVoiceEngine.a ./lib/libaec.a ./lib/libresampler.a ./lib/libanr.a ./lib/libisp.a ./lib/libsns_ov9712.a ./lib/lib_hiae.a ./lib/lib_hiawb.a ./lib/lib_hiaf.a -lpthread -lm \
		-Dhi3518 -DHICHIP=0x35180100 -DSENSOR_TYPE=OMNI_OV9712_DC_720P_30FPS -DHI_DEBUG -DHI_XXXX -DISP_V2 \
		main.c ringfifo.c rtputils.c rtspservice.c rtsputils.c loadbmp.c sample_comm_audio.c sample_comm_isp.c sample_comm_sys.c sample_comm_vda.c sample_comm_venc.c sample_comm_vi.c sample_comm_vo.c sample_comm_vpss.c sample_venc.c \
		-o $(@D)/rtspd
endef

define RTSPD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/rtspd $(TARGET_DIR)/usr/sbin/rtspd
endef

$(eval $(generic-package))

