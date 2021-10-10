#include <stdio.h>
#include <stdlib.h>
#include "com_scarike_jni_demo3_Main.h"

JNIEXPORT jstring JNICALL Java_com_scarike_jni_demo3_Main_jniString
  (JNIEnv* env, jobject jobj, jstring jstr){
      const char* str=(*env)->GetStringUTFChars(env,jstr,0);
      if(str==NULL){
          printf("str is NULL");
          return NULL;
      }
      printf("C recv=> %s\n",str);
      (*env)->ReleaseStringUTFChars(env,jstr,str);

      const char* newStr="C语言中定义的String!";
      jstring newJstr=(*env)->NewStringUTF(env,newStr);
      return newJstr;
  }