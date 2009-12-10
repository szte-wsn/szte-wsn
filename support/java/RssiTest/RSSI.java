/** Copyright (c) 2009, University of Szeged
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
* Author: Zoltan Kincses
*/

import static java.lang.System.out;
import java.io.IOException;
import java.util.ArrayList;
import java.lang.Math;
import net.tinyos.message.*;
import net.tinyos.util.*;
import java.io.FileOutputStream;
import java.io.PrintStream;

class RSSI implements MessageListener {
	
	private MoteIF mif;
	private FileOutputStream fileOutput;
	private PrintStream printToFile;
				
	public RSSI()
	{
		mif = new MoteIF(PrintStreamMessenger.err);
		mif.registerListener(new ControlMsg(),this);
		mif.registerListener(new Int8Msg(),this);
		mif.registerListener(new Uint8Msg(),this);
		mif.registerListener(new Uint16Msg(),this);
			
	}
	public void messageReceived(int dest_addr,Message msg)
	{
		try{
			if (fileOutput==null && printToFile==null) {
				fileOutput= new FileOutputStream("TestResults.txt");
				printToFile = new PrintStream(fileOutput);
			}
			if(msg instanceof Int8Msg) {
				Int8Msg int8Msg = (Int8Msg)msg;
				printToFile.print("The result of the RSSI measurement between node "+int8Msg.get_senderNodeID()+" and node "+int8Msg.get_receiverNodeID()+"\n");
				printToFile.print("Minimum: " +int8Msg.get_min()+"\n");
				printToFile.printf("Average: %.2f\n",((double)int8Msg.get_sum_a()/(double)int8Msg.get_sampleCnt()));
				printToFile.print("Maximum: " +int8Msg.get_max()+"\n");
				printToFile.printf("Energy:  %.2f\n",((double)int8Msg.get_sum_e()/(double)(int8Msg.get_sampleCnt()-1)));
			}
			else if (msg instanceof Uint8Msg) {
				Uint8Msg uint8Msg = (Uint8Msg)msg;
				if(uint8Msg.get_dataType()=='l') {
					printToFile.print("The result of the LQI measurement between node "+uint8Msg.get_senderNodeID()+" and node "+uint8Msg.get_receiverNodeID()+"\n");
				} else if (uint8Msg.get_dataType()=='r'){
					printToFile.print("The result of the RSSI measurement between node "+uint8Msg.get_senderNodeID()+" and node "+uint8Msg.get_receiverNodeID()+"\n");
				}
				printToFile.print("Minimum: " +uint8Msg.get_min()+"\n");
				printToFile.printf("Average: %.2f\n",((double)uint8Msg.get_sum_a()/(double)uint8Msg.get_sampleCnt()));
				printToFile.print("Maximum: " +uint8Msg.get_max()+"\n");
				printToFile.printf("Energy:  %.2f\n",((double)uint8Msg.get_sum_e()/(double)(uint8Msg.get_sampleCnt()-1)));
			} else if (msg instanceof Uint16Msg) {
				Uint16Msg uint16Msg = (Uint16Msg)msg;
				printToFile.print("The result of the Vref measurement in case of node "+uint16Msg.get_receiverNodeID()+"\n");
				printToFile.print("Minimum: " +uint16Msg.get_min()+"\n");
				printToFile.printf("Average: %.2f\n",((double)uint16Msg.get_sum_a()/(double)uint16Msg.get_sampleCnt()));
				printToFile.print("Maximum: " +uint16Msg.get_max()+"\n");
				printToFile.printf("Energy:  %.2f\n",((double)uint16Msg.get_sum_e()/(double)(uint16Msg.get_sampleCnt()-1)));
			}
		}catch(IOException e){
			out.println("Cannot open the file");
		}
	}
	
	public void sendCtrlMessage(short nodeID,short[] instr)  
    {
    	ControlMsg ctrlmsg=new ControlMsg();
	
    	ctrlmsg.set_nodeID(nodeID);
    	ctrlmsg.set_instr(instr);
		try{
			mif.send((int)nodeID,ctrlmsg);
		}catch(IOException e)
		{
			out.println("Cannot send message to node " + nodeID );
		}
	}
	
	public void run(String nodeID,String from,String to)
	{
		try	{
			String cache;
			short[] instr= new short[2];
			instr[0]='s';
			sendCtrlMessage(Short.parseShort(nodeID),instr);
			Thread.sleep(4000);
			if(Integer.parseInt(from)>Integer.parseInt(to))
			{
				cache = from;
				from = to;
				to = cache;
			}
			for(short count = Short.parseShort(from); count<=Short.parseShort(to);count++)
			{
				instr[0]='g';
				instr[1]='r';
				sendCtrlMessage(count,instr);
				Thread.sleep(500);
				instr[1]='l';
				sendCtrlMessage(count,instr);
				Thread.sleep(500);
				instr[1]='v';
				sendCtrlMessage(count,instr);
				Thread.sleep(500);
			}
			fileOutput.close();
			printToFile.close();
			out.print("Ready!\n");
		}catch(InterruptedException e){
		}catch(IOException e){}
	}

	public static void main (String[] args)
	{
		String[] temp;
		if (args.length<2)
		{
			out.println("Usage: RSSI [controller mote ID] [motes which measure data (from-to)]");
		}
		else 
		{
			temp=args[1].split("-");
			RSSI rssitester= new RSSI();
			rssitester.run(args[0],temp[0],temp[1]);
			System.exit(0);
		}
	}	
}

