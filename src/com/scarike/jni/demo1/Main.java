package com.scarike.jni.demo1;

public class Main {
    static {
        System.loadLibrary("demo1");
    }

    private native void sayHello();

    public static void main(String[] args) {
        new Main().sayHello();
    }
}
