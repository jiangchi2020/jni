package com.scarike.jni.util;

public class ObjectUtil {

    static {
        System.loadLibrary("objectutil");
    }

    public native static String getMarkWordAsString(Object instance);

    public static void main(String[] args) {
        Object o = new Object();
        String s=getMarkWordAsString(o);
        System.out.println(s);
    }
}
