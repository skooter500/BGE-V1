VERSION = 4.44.31

all:
	@echo "Possible targets:"
	@echo "'make fmod_examples CPU=x86' - Build all 32bit examples"
	@echo "'make fmod_examples CPU=x64' - Build all 64bit examples"

fmod_examples:
	cd fmoddesignerapi/examples/media && ./build_media_files.sh
	cd examples/3d && make ${CPU}
	cd examples/cdplayer && make ${CPU}
	cd examples/channelgroups && make ${CPU}
	cd examples/dsp_custom && make ${CPU}
	cd examples/dsp_effectperspeaker && make ${CPU}
	cd examples/effects && make ${CPU}
	cd examples/filecallbacks && make ${CPU}
	cd examples/generatetone && make ${CPU}
	cd examples/loadfrommemory && make ${CPU}
	cd examples/multiplesoundcard && make ${CPU}
	cd examples/multispeakeroutput && make ${CPU}
	cd examples/netstream && make ${CPU}
	cd examples/offlinedecoding && make ${CPU}
	cd examples/pitchdetection && make ${CPU}
	cd examples/playlist && make ${CPU}
	cd examples/playsound && make ${CPU}
	cd examples/playstream && make ${CPU}
	cd examples/plugin_dev/codec_raw && make ${CPU}
	cd examples/plugin_dev/dsp_gain && make ${CPU}
	cd examples/readtags && make ${CPU}
	cd examples/realtimestitching && make ${CPU}
	cd examples/recording && make ${CPU}
	cd examples/recordtodisk && make ${CPU}
	cd examples/ripnetstream && make ${CPU}
	cd examples/submixing && make ${CPU}
	cd examples/useplugins && make ${CPU}
	cd examples/usercreatedsound && make ${CPU}
	cd fmoddesignerapi/examples/effects && make ${CPU}
	cd fmoddesignerapi/examples/info_only && make ${CPU}
	cd fmoddesignerapi/examples/load_data && make ${CPU}
	cd fmoddesignerapi/examples/max_playbacks && make ${CPU}
	cd fmoddesignerapi/examples/parameters && make ${CPU}
	cd fmoddesignerapi/examples/programmer_selected && make ${CPU}
	cd fmoddesignerapi/examples/programmer_sound && make ${CPU}
	cd fmoddesignerapi/examples/realtime_tweaking && make ${CPU}
	cd fmoddesignerapi/examples/simple_event && make ${CPU}
	@echo "done."
