/**
 * This class is automatically generated by mig. DO NOT EDIT THIS FILE.
 * This class implements a Java interface to the 'TimeMsg'
 * message type.
 */

public class TimeMsg extends net.tinyos.message.Message {

    /** The default size of this message type in bytes. */
    public static final int DEFAULT_MESSAGE_SIZE = 10;

    /** The Active Message type associated with this message. */
    public static final int AM_TYPE = 2;

    /** Create a new TimeMsg of size 10. */
    public TimeMsg() {
        super(DEFAULT_MESSAGE_SIZE);
        amTypeSet(AM_TYPE);
    }

    /** Create a new TimeMsg of the given data_length. */
    public TimeMsg(int data_length) {
        super(data_length);
        amTypeSet(AM_TYPE);
    }

    /**
     * Create a new TimeMsg with the given data_length
     * and base offset.
     */
    public TimeMsg(int data_length, int base_offset) {
        super(data_length, base_offset);
        amTypeSet(AM_TYPE);
    }

    /**
     * Create a new TimeMsg using the given byte array
     * as backing store.
     */
    public TimeMsg(byte[] data) {
        super(data);
        amTypeSet(AM_TYPE);
    }

    /**
     * Create a new TimeMsg using the given byte array
     * as backing store, with the given base offset.
     */
    public TimeMsg(byte[] data, int base_offset) {
        super(data, base_offset);
        amTypeSet(AM_TYPE);
    }

    /**
     * Create a new TimeMsg using the given byte array
     * as backing store, with the given base offset and data length.
     */
    public TimeMsg(byte[] data, int base_offset, int data_length) {
        super(data, base_offset, data_length);
        amTypeSet(AM_TYPE);
    }

    /**
     * Create a new TimeMsg embedded in the given message
     * at the given base offset.
     */
    public TimeMsg(net.tinyos.message.Message msg, int base_offset) {
        super(msg, base_offset, DEFAULT_MESSAGE_SIZE);
        amTypeSet(AM_TYPE);
    }

    /**
     * Create a new TimeMsg embedded in the given message
     * at the given base offset and length.
     */
    public TimeMsg(net.tinyos.message.Message msg, int base_offset, int data_length) {
        super(msg, base_offset, data_length);
        amTypeSet(AM_TYPE);
    }

    /**
    /* Return a String representation of this message. Includes the
     * message type name and the non-indexed field values.
     */
    public String toString() {
      String s = "Message <TimeMsg> \n";
      try {
        s += "  [remNodeID=0x"+Long.toHexString(get_remNodeID())+"]\n";
      } catch (ArrayIndexOutOfBoundsException aioobe) { /* Skip field */ }
      try {
        s += "  [remTime=0x"+Long.toHexString(get_remTime())+"]\n";
      } catch (ArrayIndexOutOfBoundsException aioobe) { /* Skip field */ }
      try {
        s += "  [locNodeID=0x"+Long.toHexString(get_locNodeID())+"]\n";
      } catch (ArrayIndexOutOfBoundsException aioobe) { /* Skip field */ }
      try {
        s += "  [locTime=0x"+Long.toHexString(get_locTime())+"]\n";
      } catch (ArrayIndexOutOfBoundsException aioobe) { /* Skip field */ }
      return s;
    }

    // Message-type-specific access methods appear below.

    /////////////////////////////////////////////////////////
    // Accessor methods for field: remNodeID
    //   Field type: short, unsigned
    //   Offset (bits): 0
    //   Size (bits): 8
    /////////////////////////////////////////////////////////

    /**
     * Return whether the field 'remNodeID' is signed (false).
     */
    public static boolean isSigned_remNodeID() {
        return false;
    }

    /**
     * Return whether the field 'remNodeID' is an array (false).
     */
    public static boolean isArray_remNodeID() {
        return false;
    }

    /**
     * Return the offset (in bytes) of the field 'remNodeID'
     */
    public static int offset_remNodeID() {
        return (0 / 8);
    }

    /**
     * Return the offset (in bits) of the field 'remNodeID'
     */
    public static int offsetBits_remNodeID() {
        return 0;
    }

    /**
     * Return the value (as a short) of the field 'remNodeID'
     */
    public short get_remNodeID() {
        return (short)getUIntBEElement(offsetBits_remNodeID(), 8);
    }

    /**
     * Set the value of the field 'remNodeID'
     */
    public void set_remNodeID(short value) {
        setUIntBEElement(offsetBits_remNodeID(), 8, value);
    }

    /**
     * Return the size, in bytes, of the field 'remNodeID'
     */
    public static int size_remNodeID() {
        return (8 / 8);
    }

    /**
     * Return the size, in bits, of the field 'remNodeID'
     */
    public static int sizeBits_remNodeID() {
        return 8;
    }

    /////////////////////////////////////////////////////////
    // Accessor methods for field: remTime
    //   Field type: long, unsigned
    //   Offset (bits): 8
    //   Size (bits): 32
    /////////////////////////////////////////////////////////

    /**
     * Return whether the field 'remTime' is signed (false).
     */
    public static boolean isSigned_remTime() {
        return false;
    }

    /**
     * Return whether the field 'remTime' is an array (false).
     */
    public static boolean isArray_remTime() {
        return false;
    }

    /**
     * Return the offset (in bytes) of the field 'remTime'
     */
    public static int offset_remTime() {
        return (8 / 8);
    }

    /**
     * Return the offset (in bits) of the field 'remTime'
     */
    public static int offsetBits_remTime() {
        return 8;
    }

    /**
     * Return the value (as a long) of the field 'remTime'
     */
    public long get_remTime() {
        return (long)getUIntBEElement(offsetBits_remTime(), 32);
    }

    /**
     * Set the value of the field 'remTime'
     */
    public void set_remTime(long value) {
        setUIntBEElement(offsetBits_remTime(), 32, value);
    }

    /**
     * Return the size, in bytes, of the field 'remTime'
     */
    public static int size_remTime() {
        return (32 / 8);
    }

    /**
     * Return the size, in bits, of the field 'remTime'
     */
    public static int sizeBits_remTime() {
        return 32;
    }

    /////////////////////////////////////////////////////////
    // Accessor methods for field: locNodeID
    //   Field type: short, unsigned
    //   Offset (bits): 40
    //   Size (bits): 8
    /////////////////////////////////////////////////////////

    /**
     * Return whether the field 'locNodeID' is signed (false).
     */
    public static boolean isSigned_locNodeID() {
        return false;
    }

    /**
     * Return whether the field 'locNodeID' is an array (false).
     */
    public static boolean isArray_locNodeID() {
        return false;
    }

    /**
     * Return the offset (in bytes) of the field 'locNodeID'
     */
    public static int offset_locNodeID() {
        return (40 / 8);
    }

    /**
     * Return the offset (in bits) of the field 'locNodeID'
     */
    public static int offsetBits_locNodeID() {
        return 40;
    }

    /**
     * Return the value (as a short) of the field 'locNodeID'
     */
    public short get_locNodeID() {
        return (short)getUIntBEElement(offsetBits_locNodeID(), 8);
    }

    /**
     * Set the value of the field 'locNodeID'
     */
    public void set_locNodeID(short value) {
        setUIntBEElement(offsetBits_locNodeID(), 8, value);
    }

    /**
     * Return the size, in bytes, of the field 'locNodeID'
     */
    public static int size_locNodeID() {
        return (8 / 8);
    }

    /**
     * Return the size, in bits, of the field 'locNodeID'
     */
    public static int sizeBits_locNodeID() {
        return 8;
    }

    /////////////////////////////////////////////////////////
    // Accessor methods for field: locTime
    //   Field type: long, unsigned
    //   Offset (bits): 48
    //   Size (bits): 32
    /////////////////////////////////////////////////////////

    /**
     * Return whether the field 'locTime' is signed (false).
     */
    public static boolean isSigned_locTime() {
        return false;
    }

    /**
     * Return whether the field 'locTime' is an array (false).
     */
    public static boolean isArray_locTime() {
        return false;
    }

    /**
     * Return the offset (in bytes) of the field 'locTime'
     */
    public static int offset_locTime() {
        return (48 / 8);
    }

    /**
     * Return the offset (in bits) of the field 'locTime'
     */
    public static int offsetBits_locTime() {
        return 48;
    }

    /**
     * Return the value (as a long) of the field 'locTime'
     */
    public long get_locTime() {
        return (long)getUIntBEElement(offsetBits_locTime(), 32);
    }

    /**
     * Set the value of the field 'locTime'
     */
    public void set_locTime(long value) {
        setUIntBEElement(offsetBits_locTime(), 32, value);
    }

    /**
     * Return the size, in bytes, of the field 'locTime'
     */
    public static int size_locTime() {
        return (32 / 8);
    }

    /**
     * Return the size, in bits, of the field 'locTime'
     */
    public static int sizeBits_locTime() {
        return 32;
    }

}