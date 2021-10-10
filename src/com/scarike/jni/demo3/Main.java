package com.scarike.jni.demo3;

public class Main {

    static{
        System.loadLibrary("demo3");
    }

    private native String jniString(String str);

    public static void main(String[] args) {
        String str=new Main().jniString("中文&English");
        System.out.println("JVM recv=> "+str);
    }
}
