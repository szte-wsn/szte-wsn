/*
* Copyright (c) 2011, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Andras Biro
*/ 

import java.net.*;
import java.io.*;

public class Forwarder {
	final static byte VERSION[] = {'U', ' '};
	
	InputStream isA, isB;
	OutputStream osA, osB;
	Thread atob, btoa;
	
	public Forwarder(int portA, int portB){
	  ServerSocket serverSocketA = null;
	  ServerSocket serverSocketB = null;

	  Socket clientSocketA = null, clientSocketB = null;
	  

	  try {
		  serverSocketA = new ServerSocket(portA);
	  } catch (IOException e) {
		  System.err.println("Could not listen on port:" + portA + ".");
		  System.exit(1);
	  }
	  try {
		  serverSocketB = new ServerSocket(portB);
	  } catch (IOException e) {
		  System.err.println("Could not listen on port:" + portB + ".");
		  System.exit(1);
	  }
	  try {
		  clientSocketA = serverSocketA.accept();
		  clientSocketB = serverSocketB.accept();
	  } catch (IOException e) {
		  System.err.println("Accept failed.");
		  System.exit(1);
	  }
	  
	  System.out.println("Connected");
	  try{
	    isA=clientSocketA.getInputStream();
	    isB=clientSocketB.getInputStream();
	    osA=clientSocketA.getOutputStream();
	    osB=clientSocketB.getOutputStream();
	  }catch(IOException e){
	    System.err.println("Can't open streams.");
	    System.exit(1);
	  }
	  
	  try{
	    osA.write(VERSION);
	    osB.write(VERSION);
	    if(isA.read()!=VERSION[0]||isA.read()!=VERSION[1]||isB.read()!=VERSION[0]||isB.read()!=VERSION[1]){
	      System.err.println("Version error.");
	      System.exit(1);
	    }
	  }catch(IOException e){
	    System.err.println("Can't read/write streams.");
	    System.exit(1);
	  }
	  System.err.println("Version OK. Starting forwarding");
	}
	
	public class IsToOs implements Runnable{
	  InputStream is;
	  OutputStream os;
	  
	  public IsToOs(InputStream is, OutputStream os){
	    this.is=is;
	    this.os=os;
	  }
	  
	  public void run(){
	     do{
	       try{
		os.write(is.read());
	       }catch(IOException e){
		System.err.println("Can't read/write streams.");
		System.exit(1);
	       }
	     }while(true);
	  }
	}
	
	public void start(){
	  atob=new Thread(new IsToOs(isA,osB),"AtoB");
	  btoa=new Thread(new IsToOs(isB,osA),"BtoA");
	  atob.start();
	  btoa.start();
	}
	  
	public static void main(String[] args) throws IOException {
	  
	  if(args.length!=2){
	    System.out.println("Usage: java server [port A] [port B]");
	    System.exit(1);
	  }
	  int portA=-1, portB=-1;
	  try{
	    portA=Integer.parseInt(args[0]);
	    portB=Integer.parseInt(args[1]);
	  } catch(NumberFormatException e) {
	    System.out.println("Usage: java server [port A] [port B]");
	    System.exit(1);
	  }
	  new Forwarder(portA,portB).start();
// 	  isA.close();
// 	  isB.close();
// 	  osA.close();
// 	  osB.close();
// 	  clientSocketA.close();
// 	  clientSocketB.close();
// 	  serverSocketA.close();
// 	  serverSocketB.close();
	}
}