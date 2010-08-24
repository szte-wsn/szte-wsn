/*
 * Copyright (c) 2010, University of Szeged
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
 * Author: Miklos Toth
 */
package org.szte.wsn.dataprocess.string;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.szte.wsn.dataprocess.PacketParser;
import org.szte.wsn.dataprocess.PacketParserFactory;
import org.szte.wsn.dataprocess.StringInterface;
import org.szte.wsn.dataprocess.Usage;

/**
 * 
 * @author Miklos Toth
 *	implements StringInterface
 *	writes and reads Console
 */
public class StringInterfaceConsole implements StringInterface {
	String separator;
	String previous;
	PacketParser[] packetParsers;
	boolean showName;

	public StringInterfaceConsole(String separator, PacketParser[] packetParsers, boolean showName){
		this.separator=separator;
		this.packetParsers=packetParsers;
		previous="";
		this.showName=showName;
	}


	@Override
	/**
	 * implements writePacket for Console application
	 * 
	 */
	public void writePacket(StringPacket packet) {
		if (packet.getData()!=null){			
			if(!packet.getName().equals(previous)){
				if(showName)
					System.out.print(packet.getName()+separator);
				for(String head:packet.getFields())
					System.out.print(head+separator);

				System.out.println();			
			}
			if(showName)
				System.out.print(packet.getName()+separator);
			for(String data:packet.getData()){			
				System.out.print(data+separator);
			}
			System.out.println();
			previous=packet.getName();
		}
	}


	@Override
	/**
	 * implements readPacket for Console application
	 */
	public StringPacket readPacket() {
		StringPacket ret=null;
		try {
			BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
			System.out.println("Give me the name of the struct:");
			System.out.flush();
			String structName=in.readLine();			
			PacketParser pp=PacketParserFactory.getParser(structName, packetParsers );				
			if(pp!=null){
				if(pp.getFields().length==0)
					return new StringPacket(pp.getName(),new String[0]);				

				System.out.println("Do you want to give the fields in custom order?(y/n)");
				String customOrder=in.readLine();
				if(customOrder.equalsIgnoreCase("n"))
				{
					System.out.println("Give me one data line, with "+pp.getFields().length+" values seperated with: "+separator);
					System.out.flush();
					String[] parts=in.readLine().split(separator);
					if(parts.length==pp.getFields().length)
						ret=new StringPacket(structName,parts);
					else{
						System.out.print("Input error");
						Usage.usageThanExit();
					}	

				}
				else{
					System.out.println("Give me the fields in your custom order, seperated with: "+separator);
					for(String str:pp.getFields())
						System.out.print(str+",");

					System.out.flush();
					String[] fields=in.readLine().split(separator);
					if(fields.length!=pp.getFields().length)
						Usage.usageThanExit();
					System.out.println("Give me the data in the order of the fields above, seperated with: "+separator);
					String[] data=in.readLine().split(separator);
					if(data.length!=pp.getFields().length)
						Usage.usageThanExit();
					String temp[]=new String[data.length];
					for(int i=0;i<fields.length;i++)
						temp[i]="";
					for(int j=0;j<fields.length;j++)
						for(int i=0;i<fields.length;i++)
							if(fields[j].equals(pp.getFields()[i]))
								temp[j]=data[i];								
					ret=new StringPacket(structName,temp);
				}
			}
			else{
				System.out.println("Not existing struct.");
				Usage.usageThanExit();
			}
		}
		catch (IOException e)
		{
			e.printStackTrace();
			Usage.usageThanExit();
		} 		
		return ret;
	}

}
