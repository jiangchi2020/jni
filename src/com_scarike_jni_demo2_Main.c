/*
 * =====================================================================================
 *
 *       Filename:  com_scarike_jni_demo2_Main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021年10月08日 20时45分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include "com_scarike_jni_demo2_Main.h"
JNIEXPORT void JNICALL Java_com_scarike_jni_demo2_Main_eleType(JNIEnv * env, jobject obj, jbyte b, jshort s, jint i, jlong l, jfloat f, jdouble d, jboolean bo, jchar c){
	printf("b=%d\ns=%d\ni=%ld\nl=%lld\nf=%f\nd=%lf\nbo=%d\nc=%c\n",b,s,i,l,f,d,bo,c);
}
