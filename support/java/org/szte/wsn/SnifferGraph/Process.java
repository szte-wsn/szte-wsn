/** Copyright (c) 2010, University of Szeged
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
 * 
 * @author Nyilas Sandor Karoly , Nemeth Gabor
 * 
 * This class will get datas from sniffer and used after process 
 *
 */

package org.szte.wsn.SnifferGraph;
import java.awt.Color;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;

import javax.swing.JCheckBox;

public class Process {
	
	//private static ArrayList<JPanel> dataPanel = SnifferGraph.dataPanel;
	//private static JPanel backgPanel = SnifferGraph.backgPanel;
	//private static ArrayList<Packages> motes = SnifferGraph.motes;
	//private static JPanel centerPanel = SnifferGraph.centerPanel;
	//private static JPanel forBox = SnifferGraph.forBox;
	private static ArrayList<JCheckBox> devices = SnifferGraph.devices;
	private static ArrayList<String> dataTypes = SnifferGraph.dataTypes;
	
	//private static JCheckBox box[] = SnifferGraph.box;
	//private static Date date = new Date();
	public static int currentlyMillisec = Integer.valueOf((int) (System.currentTimeMillis()%100));
	
	public static String whatTimeIsNow(){
		return String.valueOf(secundum()) + "s : " +String.valueOf(milliSec()) + "ms";
	}
	
	private static String secundum() {
		Calendar cal = new GregorianCalendar();
		long k = cal.get(Calendar.SECOND);
		return String.valueOf(k);
	}

	public static int milliSec(){
		Calendar cal = new GregorianCalendar();
		int k = cal.get(Calendar.MILLISECOND);
		return k;
	}

	public static String dataCodeing(int i, int j) {
		return "Meg semmi";
	}

	public static String makeTimeLineSting(int i) {
		i++;
		if(i<10) return "000" + String.valueOf(i);
		else if(i<100) return "00" + String.valueOf(i);
		else if(i<1000) return "0" + String.valueOf(i);
		return "over";
	}
	
	public static String whatMoteTimeNow(){
		return "#" + whatTimeIsNow();
	}

	public static void addDevice(String s, Color color) {
		System.out.println("adding now");
		int pos = 0;
		for(pos = 0; pos<devices.size(); pos++){
			if(devices.get(pos).getText().equals("<none>")){
				break;
			}
		}
		devices.get(pos).setText(s);
		devices.get(pos).setVisible(true);
		devices.get(pos).setSelected(true);
		devices.get(pos).setBackground(color);
	}

	public static void addANewType(String s) {
		dataTypes.add(s);
	}

	/*public static void reDraw() {
		for(int i = 0; i<dataPanel.size();i++){
			dataPanel.get(i).setVisible(false);
			backgPanel.remove(dataPanel.get(i));
		}
		int length = dataPanel.size();
		for(int i = 0; i<length; i++){
			dataPanel.remove(0);
		}
		for(int i = 0; i<motes.size();i++){
			dataPanel.add(motes.get(i).getAPanel(box));
			dataPanel.get(dataPanel.size()-1).setVisible(true);
			backgPanel.add(dataPanel.get(dataPanel.size()-1));
		}
		SnifferGraph.scrollbar.setMaximum((motes.get(motes.size()-1).firstPos));
	}*/

}