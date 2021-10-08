/*
 * =====================================================================================
 *
 *       Filename:  com_scarike_jni_demo1_Main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2021年10月08日 20时25分58秒
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
#include "com_scarike_jni_demo1_Main.h"

JNIEXPORT void JNICALL Java_com_scarike_jni_demo1_Main_sayHello(JNIEnv * env, jobject obj){
	printf("Hello JNI!");
}
