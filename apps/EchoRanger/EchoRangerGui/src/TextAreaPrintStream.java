/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author csepezoltan
 */
import java.io.*;
import javax.swing.*;

public class TextAreaPrintStream extends PrintStream {

    //The JTextArea to wich the output stream will be redirected.
    private JTextArea textArea;


    /**
     * Method TextAreaPrintStream
     * The constructor of the class.
     * @param the JTextArea to wich the output stream will be redirected.
     * @param a standard output stream (needed by super method)
     **/
    public TextAreaPrintStream(JTextArea area, OutputStream out) {
	super(out);
	textArea = area;
    }

    /**
     * Method println
     * @param the String to be output in the JTextArea textArea (private
     * attribute of the class).
     * After having printed such a String, prints a new line.
     **/
    @Override
    public void println(String string) {
	textArea.append(string+"\n");
    }

}
