/** 
 * Copyright (c) 1998, 2021, Oracle and/or its affiliates. All rights reserved.
 * 
 */


package hwb1;

import javacard.framework.*;
import javacard.security.*;
import javacardx.crypto.*;

/**
 * Applet class
 * 
 * @author <dolanluk>
 */

public class FirstApplet extends Applet {
	
	static final byte [] enc_key_data = {
			0x00, 0x01, 0x02, 0x03,
			0x00, 0x01, 0x02, 0x03,
			0x00, 0x01, 0x02, 0x03,
			0x00, 0x01, 0x02, 0x03
			};
	static final byte [] mac_key_data = {
			0x0a, 0x01, 0x02, 0x03,
			0x0b, 0x01, 0x02, 0x03,
			0x0c, 0x01, 0x02, 0x03,
			0x0d, 0x01, 0x02, 0x03
			};
	
	AESKey key_enc;
	AESKey key_mac;
	
	Cipher enc;
	Signature mac;
	
	static final byte [] name = {'L', 'u', 'k', 'a', 's'};
	byte my_data[] = new byte[20];
	short receive = 0;
	
	OwnerPIN pin;
	final static short SW_VERIFICATION_FAILED = 0x6300;
	final static short SW_PIN_VERIFICATION_REQUIRED = 0x6301;
	
    /**
     * Installs this applet.
     * 
     * @param bArray
     *            the array containing installation parameters
     * @param bOffset
     *            the starting offset in bArray
     * @param bLength
     *            the length in bytes of the parameter data in bArray
     */
    public static void install(byte[] bArray, short bOffset, byte bLength) {
        new FirstApplet(bArray, bOffset, bLength);
    }

    /**
     * Only this class's install method should create the applet object.
     */
    //protected FirstApplet() {
    protected FirstApplet(byte[] bArray, short bOffset, byte bLength) {
    	byte iLen = bArray[bOffset]; // aid length
        bOffset = (short) (bOffset + iLen + 1);
        byte cLen = bArray[bOffset]; // info length
        bOffset = (short) (bOffset + cLen + 1);
        byte aLen = bArray[bOffset]; // applet data length
        bOffset = (short) (bOffset + 1); // applet data
        
        pin = new OwnerPIN((byte) 3, (byte) 4);
        pin.update(bArray, bOffset, aLen);

        key_enc = (AESKey) KeyBuilder.buildKey(KeyBuilder.TYPE_AES, KeyBuilder.LENGTH_AES_128, false);
        key_enc.setKey(enc_key_data, (short)0);
        
        key_mac = (AESKey) KeyBuilder.buildKey(KeyBuilder.TYPE_AES, KeyBuilder.LENGTH_AES_128, false);
        key_mac.setKey(mac_key_data, (short)0);
        
        enc = Cipher.getInstance(Cipher.ALG_AES_BLOCK_128_CBC_NOPAD, false);
        mac = Signature.getInstance(Signature.ALG_AES_MAC_128_NOPAD, false);
        
    	register();
    }

    // send name
    private void sendName(APDU apdu)
    {
    	short len;
    	// save expected length
		len = apdu.setOutgoing();
		// set the correct length
		len = (short)name.length;
		// set length to send
		apdu.setOutgoingLength(len);
		// send data
		apdu.sendBytesLong(name, (short)0, len);
    }
    
    // receive data, max 20 bytes
    private void receiveData(APDU apdu)
    {
    	byte buffer[] = apdu.getBuffer();
    	// save expected length
		short len = apdu.setIncomingAndReceive();
		// if more than buffer, throw exception ---save maximum allowed
		if(len > 20) ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
		// save received length
		receive = len;
		// store data
		Util.arrayCopy(buffer, ISO7816.OFFSET_CDATA, my_data, (short)0, len);
    }
    
    // send previously received data
    private void sendData(APDU apdu)
    {
    	// save expected length
		short len = apdu.setOutgoing();
		// check receive length
		if(len != receive) ISOException.throwIt((short)(ISO7816.SW_CORRECT_LENGTH_00 + receive)); 
		// save length to send
		apdu.setOutgoingLength(len);
		// send data
		apdu.sendBytesLong(my_data, (short)0, len);
    }
    
    // check PIN
    private void checkPin(APDU apdu)
    {
    	short len = apdu.setIncomingAndReceive();
    	byte buffer[] = apdu.getBuffer();
		if (!pin.check(buffer, ISO7816.OFFSET_CDATA, (byte) len))
		{
			ISOException.throwIt(SW_VERIFICATION_FAILED);
		}
    }
    
    // encrypt data
    private void cipherData(APDU apdu)
    {
    	byte buffer[] = apdu.getBuffer();
    	short len_enc = apdu.setIncomingAndReceive();
    	if(len_enc > 64) ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);

   		enc.init(key_enc, Cipher.MODE_ENCRYPT);
		
		try {
			enc.doFinal(buffer, ISO7816.OFFSET_CDATA, len_enc, buffer, (short)0);
		} catch(CryptoException e) {
			// not block aligned data
			if(e.getReason() == CryptoException.ILLEGAL_USE) ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
		}
		// calculate MAC
		mac.init(key_mac, Signature.MODE_SIGN);
		
		short len_mac = mac.sign(buffer, (short)0, len_enc, buffer, len_enc);
		// send encrypted and signed data
		apdu.setOutgoingAndSend((short)0, (short)(len_enc + len_mac));
    }
    
    // decrypt data
    private void decipherData(APDU apdu)
    {
    	byte buffer[] = apdu.getBuffer();
    	short len = apdu.setIncomingAndReceive();
    	short len_mac = 16;
    	
    	// calculate data length
    	if((len < len_mac ) || (len > 80)) ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
    	short len_enc = (short)(len - len_mac);
    	
    	// verify signature
    	mac.init(key_mac, Signature.MODE_VERIFY);
    	try {
    		if(!mac.verify(buffer, ISO7816.OFFSET_CDATA, len_enc, buffer, (short)(ISO7816.OFFSET_CDATA + len_enc), len_mac))
    		{
    			ISOException.throwIt(ISO7816.SW_WRONG_DATA);
    		}
    	} catch (CryptoException e) {
    		ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
    	}
    	enc.init(key_enc, Cipher.MODE_DECRYPT);
    	try {
    		enc.doFinal(buffer, ISO7816.OFFSET_CDATA, len_enc, buffer, (short)0);
    	} catch (CryptoException e) {
    		ISOException.throwIt(ISO7816.SW_WRONG_LENGTH);
    	}
    	apdu.setOutgoingAndSend((short)0, len_enc);
    }
    
    /**
     * Processes an incoming APDU.
     * 
     * @see APDU
     * @param apdu
     *            the incoming APDU
     */

    public void process(APDU apdu) {
    	// TODO:
    		// dodelat testovani
    		// nefunguje CAP file
    	if (selectingApplet()) ISOException.throwIt(ISO7816.SW_NO_ERROR);  	
    	byte buffer[] = apdu.getBuffer();
    	// check class
    	if ((buffer[ISO7816.OFFSET_CLA]&0xff) != 0x80) ISOException.throwIt(ISO7816.SW_CLA_NOT_SUPPORTED);
    	switch (buffer[ISO7816.OFFSET_INS]) {
    		// send name
    		case 0x00:
    			sendName(apdu);
    			break;
    		// receive data, max 20 bytes
    		case 0x02:
    			if(!pin.isValidated()) ISOException.throwIt(SW_PIN_VERIFICATION_REQUIRED);
    			receiveData(apdu);
    			break;
    		// send data
    		case 0x04:
    			if(!pin.isValidated()) ISOException.throwIt(SW_PIN_VERIFICATION_REQUIRED);
    			sendData(apdu);
    			break;
    		// check pin
    		case 0x20:
    			checkPin(apdu);
    			break;
    		//cipher data, max 64 bytes
    		case 0x42:
    			cipherData(apdu);
    			break;
    		// decipher data, max 80 bytes (64 data + 16 MAC)
    		case 0x44:
    			decipherData(apdu);
    			break;
    		// invalid INS
    		default:
    	    	ISOException.throwIt(ISO7816.SW_INS_NOT_SUPPORTED);
    			break;
    	}
    }
}
