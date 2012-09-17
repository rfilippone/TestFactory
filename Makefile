.PHONY: all install clean

.DEFAULT_GOAL:= all

config ?= debug
CXX_FLAGS:=-Wall

INCLUDES:=-Isrc $(BOOST_INC)

LD_FLAGS:=$(BOOST_LIB_DIR)
LIBS:=

build_dir=build
config_dir=$(build_dir)/$(config)
objs_dir=$(config_dir)/objs

$(build_dir):
	mkdir -p $@

$(config_dir):
	mkdir -p $@

$(objs_dir):
	mkdir -p $@


###### executable

exe_target = TestFactory
exe_srcs_dir = src
exe_srcs:=$(foreach dir,$(exe_srcs_dir), $(wildcard $(dir)/*.cpp))
exe_artifact=$(build_dir)/$(config)/$(exe_target)
exe_install_dest=web/public/$(exe_target)

exe_objs:= $(exe_srcs:%.cpp=$(objs_dir)/%.o)
exe_objs_subdirs:=$(sort $(dir $(exe_objs)))

$(exe_objs_subdirs):
	mkdir -p $@

###### common

all: install

install: $(exe_artifact)

$(exe_artifact): $(objs_dir) $(exe_objs_subdirs) $(exe_objs) $(lib_artifact)
	$(CXX) $(exe_objs) -o $@ $(LD_FLAGS) $(LIBS)

$(objs_dir)/%.o : %.cpp
	$(CXX) -c $< -o $@ $(CXX_FLAGS) $(INCLUDES) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$@"

clean:
	rm -f $(exe_artifact)
	rm -fr $(objs_dir)
	rm -fr $(config_dir)
	rm -fr $(build_dir)

-include $(foreach obj,$(exe_objs), $(obj:%.o=%.d))
