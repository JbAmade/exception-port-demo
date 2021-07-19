//
//  main.c
//  exceptions
//
//  Created by Jb on 18/07/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <mach/mach_error.h>
#include <signal.h>
#include <mach/mach.h>
#include <pthread/pthread.h>
#include <unistd.h>

int launchBinary(pid_t *pid, char *binary, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6, char**env){
    const char *args[]={binary, arg1, arg2, arg3, arg4, arg5, arg6, NULL};
    int rv = posix_spawn(pid, binary, NULL, NULL, (char **)&args, env);
    printf("Posix spawn returned %s for %s\n", mach_error_string(rv), binary);
    if(rv) return rv;
    return 0;
}

void *exceptionHandler(mach_port_t exception_port){
    uint32_t msg_size = 0x1000;
    mach_msg_header_t *msg = malloc(msg_size);
    kern_return_t ret;
    for(;;){
        printf("Hello from thread ! Waiting for an exception message !\n");
        ret = mach_msg(msg, MACH_RCV_MSG|MACH_MSG_TIMEOUT_NONE, 0, msg_size, exception_port, 0, 0);
        if (ret!=KERN_SUCCESS){
            printf("[-] Failed to receive exception message : %s\n", mach_error_string(ret));
            return NULL;
        }
        printf("[+] Received exception message !\n");
    }
    return NULL;
}

extern char **environ;
int main(int argc, const char * argv[]) {
    // insert code here...
    pid_t execPid = 0;
    launchBinary(&execPid, "child", NULL, NULL, NULL, NULL, NULL, NULL, environ);
    printf("[i] execPid : %d\n", execPid);
    
    mach_port_t exec_task_port = MACH_PORT_NULL;
    kern_return_t ret = task_for_pid(mach_task_self(), execPid, &exec_task_port);
    if(ret!=KERN_SUCCESS){
        printf("[-] Failed to get child's task port\n");
        goto end;
    }
    printf("[+] Success got child's task port : 0x%x\n", exec_task_port);
    
    mach_port_t exception_port;
    ret = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &exception_port);
    if(ret!=KERN_SUCCESS){
        printf("[-] Failed to allocate Mach port\n");
        goto end;
    }
    printf("[+] Success allocating Mach port\n");
    
    ret = mach_port_insert_right(mach_task_self(), exception_port, exception_port, MACH_MSG_TYPE_MAKE_SEND);
    if(ret!=KERN_SUCCESS){
        printf("[-] Failed to insert right\n");
        goto end;
    }
    printf("[+] Success inserted right\n");

#if __arm64__
    ret = task_set_exception_ports(exec_task_port, EXC_MASK_BAD_ACCESS, exception_port, EXCEPTION_DEFAULT, ARM_THREAD_STATE64);
#else
    ret = task_set_exception_ports(exec_task_port, EXC_MASK_BAD_ACCESS, exception_port, EXCEPTION_DEFAULT, x86_THREAD_STATE64);
#endif
    if(ret!=KERN_SUCCESS){
        printf("[-] Failed to set exception port\n");
        goto end;
    }
    printf("[+] Success setting exception port\n");
    
    pthread_t thread;
    ret = pthread_create(&thread, NULL, exceptionHandler(exception_port), NULL);
    if(ret!=KERN_SUCCESS){
        printf("[-] Failed to start thread\n");
        goto end;
    }
    printf("[+] Success starting thread\n");
    printf("Done !\n");
end:;
    kill(execPid, SIGKILL);
    return 0;
}
