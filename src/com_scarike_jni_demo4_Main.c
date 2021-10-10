#include <stdlib.h>
#include <stdio.h>
#include "com_scarike_jni_demo4_Main.h"

int comp(jint* o1,jint* o2){
    return *o1-*o2;
}

JNIEXPORT jdoubleArray JNICALL Java_com_scarike_jni_demo4_Main_jniArray
  (JNIEnv * env, jobject jobj, jintArray jints){
      printf("=============native===========\n");
      jboolean mark=JNI_FALSE;
      jint* cintArray=(*env)->GetIntArrayElements(env,jints,&mark);
      if(cintArray==NULL){
          printf("array is NULL\n");
          return NULL;
      }
      printf("mark is %s\n",mark?"JNI_TRUE":"JNI_FALSE");
      int size=(*env)->GetArrayLength(env,jints);
      printf("size is %d\n",size);
      double avg,sum=0;

      printf("recv array=> [");
      for(int i=0;i<size;++i){
          printf("%d%c",cintArray[i],",]"[i==size-1]);
          sum+=cintArray[i];
      }
      printf("\n");
      avg=sum/size;
      qsort(cintArray,size,sizeof(jint),comp);
      printf("sorted array=> [");
      for(int i=0;i<size;++i){
          printf("%d%c",cintArray[i],",]"[i==size-1]);
          sum+=cintArray[i];
      }
      printf("\n");
      (*env)->ReleaseIntArrayElements(env,jints,cintArray,0);
      
      jdouble res[2];
      res[0]=sum,res[1]=avg;
      jdoubleArray jdoubles=(*env)->NewDoubleArray(env,2);
      if(jdoubles==NULL){
          printf("jdoubles is NULL\n");
          return NULL;
      }
      (*env)->SetDoubleArrayRegion(env,jdoubles,0,2,res);
      printf("=============native===========\n");
      return jdoubles;
  }
