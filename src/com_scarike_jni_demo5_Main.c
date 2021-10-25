#include <stdlib.h>
#include <stdio.h>
#include "com_scarike_jni_demo5_Main.h"

JNIEXPORT jobject JNICALL Java_com_scarike_jni_demo5_Main_jniObject
  (JNIEnv * env, jobject instance, jobject jobj){
    printf("==========native========\n");

    jclass mainClass=(*env)->GetObjectClass(env,instance);
    jclass userClass=(*env)->GetObjectClass(env,jobj);
    jfieldID main_num=(*env)->GetFieldID(env,mainClass,"num","I");
    jfieldID main_user=(*env)->GetFieldID(env,mainClass,"user","Lcom/scarike/jin/demo5/User;");
    jfieldID user_name=(*env)->GetFieldID(env,userClass,"name","Ljava/lang/String;");

    jint num=(*env)->GetIntField(env,instance,main_num);
    printf("Main.num=%d,let's change its value to 114514\n",num);
    (*env)->SetIntField(env,instance,main_num,114514);

    jobject user=(*env)->GetObjectField(env,instance,main_user);
    jstring username=(*env)->GetObjectField(env,user,user_name);
    jstring uname=(*env)->GetObjectField(env,jobj,user_name);

    printf("==========native========\n");
  }

