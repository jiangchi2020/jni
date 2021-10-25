#include "com_scarike_jni_util_ObjectUtil.h"
#include <stdio.h>
/*
 * 获取对象的头部信息,64位   
*/
JNIEXPORT jstring JNICALL Java_com_scarike_jni_util_ObjectUtil_getMarkWordAsString(JNIEnv *env, jclass clazz, jobject instance)
{
    char head[72] = {0};
    unsigned char *ptr = (unsigned char *)instance;
    int index = 70;
    for(int i=0;i<7;i++){
        unsigned char value=ptr[i];
        for(int j=7;j>=0;j--){
            if((value>>j)&1){
                head[index]='1';
                printf("1");
            }else{
                head[index]='0';
                printf("0");
            }
            index--;
        }
        printf(" ");
        if(index>0){
            head[index]=' ';
            index--;
        }
    }
    printf("%s\n",head);
    return (*env)->NewStringUTF(env,head);
}
