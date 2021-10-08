package com.scarike.jni.demo2;

public class Main {
    static {
        System.loadLibrary("demo2");
    }

    private native void eleType(byte b,short s,int i,long l,float f,double d,boolean bo,char c);

    public static void main(String[] args) {
        new Main().eleType((byte) 123,(short) 345,31415926,999999999999999999l,3.14f,3.14159265,true,'c');
    }
}
