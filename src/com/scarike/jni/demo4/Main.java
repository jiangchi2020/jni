package com.scarike.jni.demo4;

import java.util.Arrays;

public class Main {
    static{
        System.loadLibrary("demo4");
    }

    private native double[] jniArray(int[] intArray);

    public static void main(String[] args) {
        int[] intArray={4,2,5,7,8,9,0,3,1,6,7};
        double[] res=new Main().jniArray(intArray);
        System.out.println("sum="+res[0]);
        System.out.println("avg="+res[1]);
        System.out.println("Array=> "+Arrays.toString(intArray));
    }
}
