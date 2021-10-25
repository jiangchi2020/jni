package com.scarike.jni.demo5;

public class Main {
    private int num;
    private User user;
    static{
        System.loadLibrary("demo5");
    }
    private native User jniObject(User user);
    public static void main(String[] args) {
        Main mainClass=new Main();
        mainClass.user=new User("张三");
        User u=new User("李四");
        User u2=mainClass.jniObject(u);
        System.out.println(mainClass.user);
        System.out.println(u);
        System.out.println(u2);
    }
}
class User{
    private String name;
    public String getName(){
        return name;
    }
    public void setName(String name){
        this.name=name;
    }
    public User(){

    }
    public User(String name){
        this.name=name;
    }
    public String toString(){
        return getClass().getName()+"@"+hashCode()+"{name="+name+"}";
    }
}
