LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	poc.cpp 

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libc \
        libbinder \
        libutils \
        liblog \
        libmedia \
        libsoundtrigger \
        libgui 
        	
LOCAL_C_INCLUDES := \
    $(TOP)/frameworks/native/include/media/openmax \
    $(TOP)/frameworks/av/include/media/ \
    $(TOP)/frameworks/av/media/libstagefright \
    $(call include-path-for, audio-effects) \
    $(call include-path-for, audio-utils)

LOCAL_MODULE:= poc
LOCAL_32_BIT_ONLY := true

include $(BUILD_EXECUTABLE)

