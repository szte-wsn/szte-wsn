import java.io.*;
import java.lang.*;
import net.tinyos.packet.*;
import net.tinyos.message.*;
import net.tinyos.util.*;


public class GreenHouse implements MessageListener
{
	//A szenzorlapka �s a sz�m�t�g�p k�z�tti kommunik�ci�t megval�s�t� oszt�ly
    private MoteIF mif;
	//�zenetek �rkez�s�nek id�pontj�t regisztr�l� tag
	protected java.text.SimpleDateFormat timestamp = new java.text.SimpleDateFormat("yyyy.MM.dd HH:mm:ss");
	//Konstruktor
	public GreenHouse(MoteIF mif){
		this.mif = mif;
		//V�rt �zenet t�pus�nak megad�sa
		this.mif.registerListener(new GreenHouseMsg(), this);
	}
	//mif.receiver.[msgTemplate].listener.messageReceived(int, Message)
	public /*synchronized */void messageReceived(int to, Message m){
		
		System.out.println("Message received.");
		System.out.println("Message's dataLength = " + m.dataLength());
		System.out.println("Message's baseOffset = " + m.baseOffset());
		
		if(m instanceof GreenHouseMsg)
		{
			GreenHouseMsg ghmsg = (GreenHouseMsg) m;
			
			int nid = ghmsg.get_source();
			
			System.out.println("Correct message format. Sent by mote " + nid + ".");
			//F�jlba �r�s
			File logfile;
			FileOutputStream fos = null;
			PrintWriter pw;
			
			final String delimiter = " ";
			//A szenzorlapk�hoz tartoz� logf�jl nev�nek meghat�roz�sa
			logfile = new File("log/mote_" + Integer.toString(nid) + ".txt");
			//Ha nem l�tezik, megpr�b�ljuk l�trehozni
			if( !logfile.exists() )
			{
				try
				{
					logfile.createNewFile();
					fos = new FileOutputStream( logfile );
				}
				catch(FileNotFoundException e)
				{
					System.out.println("Error: " + e.getMessage());
				}
				catch(IOException e)
				{
					System.out.println("Unable to create " + logfile + ": "+e.getMessage());
				}
				
				pw = new PrintWriter( fos );
				pw.println("Log file of mote with ID " + Integer.toString(nid));
				pw.close();
			}
			//T�nyleges adat�r�s
			try
			{
				fos = new FileOutputStream( logfile, true );
				pw = new PrintWriter( fos, true );
				
				pw.println( "--------------------------------------" );
				pw.println( timestamp.format(new java.util.Date()) );				
				
				pw.println( "ID of transmitter: " + Integer.toString(nid) );
				pw.println( "Sequence number: " + Integer.toString(ghmsg.get_seqno()) );
				pw.println( "Parent: " + Integer.toString(ghmsg.get_parent()) );
				pw.println( "Metric: " + Integer.toString(ghmsg.get_metric()) );
				
				pw.print( "Data: " );
				int[] data = ghmsg.get_data();
				String dataString = "";
				for(int i = 0; i < data.length; i++)
					dataString += Double.toString( data[i] ) + delimiter;
				pw.println(dataString);
				
				pw.close();
			}
			catch(FileNotFoundException e)
			{
				System.out.println("Error: " + e.getMessage());
			}
		}
	}
	
    public static void main(String[] args) {
		PhoenixSource phoenix = null;
		MoteIF mif = null;
		//Parancssori param�terek alapj�n adatforr�s meghat�roz�sa
		if( args.length == 0 )
		{
			phoenix = BuildSource.makePhoenix( PrintStreamMessenger.err );
		}
		else if( args.length == 2 && args[0].equals("-comm") )
		{
			phoenix = BuildSource.makePhoenix( args[1], PrintStreamMessenger.err );
		}
		else
		{
			System.err.println("Usage: java GreenHouse [-comm <source>]");
			java.lang.System.exit(1);
		}
		mif = new MoteIF(phoenix);
		//Alkalmaz�s futtat�sa
		GreenHouse app = new GreenHouse(mif);
    }
}
