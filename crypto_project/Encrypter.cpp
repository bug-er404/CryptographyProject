/**
*  Encrypter.cpp
*  Implementation of Encrypter.hpp
*
*
*  Created by Salil D. Maharjan on 10/10/19.
*  Copyright © 2019 Salil Maharjan. All rights reserved.
*/
#include "Encrypter.hpp"
#include <fstream>
#include "PrefixHeader.pch"

// PUBLIC METHODS:
// *********************************************************
// ************** Default Constructor **********************
// *********************************************************
Encrypter::Encrypter(){}
// *********************************************************
// ************** Default Destructor ***********************
// *********************************************************
Encrypter::~Encrypter(){}

/**
* Encrypter::EncryptFile. Encrypter method.
* Method to run the encrypter on message.
* @param a_fileName string Target file to get message to encrypt.
* @author Salil Maharjan
* @date 10/10/19.
*/
void Encrypter::EncryptFile(string a_fileName)
{
    // Variable to hold encrypted message in each step
    string encrypt;
    
    // Output formatting
    cout<<left<<endl;
    
    // Read the message for encryption and record it:
    readMessage(a_fileName);
//        cout<<setw(35)<<"Message is: "<<m_message <<endl;
    
    // Generate random key to encrypt text:
    generateKey();
        cout<<setw(35)<<"Key is: "<<m_key <<endl<<endl;
    
    // (1) Vigenere encryption with last part of 40 bit key. (Last 5 bytes):
    encrypt = vigenereCipher(m_message, 11, 16);
//        cout<<setw(35)<<"Vigenere encryption(1): "<<encrypt<<endl<<endl;
    // S-BOX Substitution part:
    encrypt = sBoxSubstitution(encrypt);
//        cout<<setw(35)<<"S-BOX substitution: "<<encrypt<<endl<<endl;
    // (2) Vigenere encryption with first part of 40 bit key. (First 5 bytes):
    encrypt = vigenereCipher(encrypt, 0, 4);
//        cout<<setw(35)<<"Vigenere encryption(2): "<<encrypt<<endl<<endl;
    // P-BOX Permuation part:
    encrypt = pboxPermutaation(encrypt);
//        cout<<setw(35)<<"P-BOX permutation: "<<encrypt<<endl<<endl;
    // (3) Vigenere encryption with bytes 5-9:
    encrypt = vigenereCipher(encrypt, 5, 9);
//        cout<<setw(35)<<"Vigenere encryption(3): "<<encrypt<<endl<<endl;
    // Caesar's cipher encryption:
    encrypt = caesarCipher(encrypt);
//        cout<<setw(35)<<"Caesar Cipher: "<<encrypt<<endl<<endl;
    // 5 Block Feistel style encryption:
    vector<bitset<8>> encryption = fiveBlockFeistel(encrypt);
//        cout<<setw(35)<<"----5B custom Feistel encryption written to file-----"<<endl<<endl;
    
    // Writing final encryption to a file.
    writeEncryption(encryption);
}


// PRIVATE METHODS:
/**
* Encrypter::readMessage. Message reader method.
* Method to read the message and record it.
* @param a_fileName string Target file to get message from
* @author Salil Maharjan
* @date 10/10/19.
*/
void Encrypter::readMessage(string a_fileName)
{
    // Temp variable to hold message
    string message = "";
    
    // Buffer to read the message
    string buffer;
    
    // Open file and check if it opened correctly.
    ifstream inf(a_fileName);
    if(!inf)
    {
        cerr << "Could not open the file: " << a_fileName << endl;
        exit(1);
    }
    
    // Getting the message to encrypt from the file.
    while(!inf.eof())
    {
        // Get each line
        getline(inf,buffer);
        message += buffer;
    }
    m_message = message;
}

/**
* Encrypter::writeEncryption. Encryption writer method.
* Method to write the encrypted binary message to a file.
* @param a_encryption vector<bitset<8>> Target encrypted message in binary to write to a file.
* @author Salil Maharjan
* @date 10/10/19.
*/
void Encrypter::writeEncryption(vector<bitset<8>> a_encryption)
{
    // Writing encrypted message to file - encrypted.txt
    ofstream opf;
    opf.open("encrypted.txt", ios::out | ios::binary);
    for(int i=0; i<a_encryption.size();i++)
    {
        opf<<a_encryption[i];
    }
    opf.close();
}

/**
* Encrypter::randomStringGenerator. Random string generator method.
* Method to randomly generate an alpha numeric string of size 'size'.
* @param size int Length of random string to generate
* @return string Random string of 'size' length.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Encrypter::randomStringGenerator(int size)
{
    // Temp string to hold random string
    string temp="";
    
    // Array of alpha numeric values
    static const char alphaNumSet[] =
    "0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    // Generate random string
    for(int i = 0; i<size; i++)
        temp += alphaNumSet[rand()%(strlen(alphaNumSet))];
    
    return temp;
}

/**
* Encrypter::generateKey. Random key generator method
* Method to randomly generate a key for encryption purposes.
* @see randomStringGenerator()
* @author Salil Maharjan
* @date 10/10/19.
*/
void Encrypter::generateKey()
{
    // Temp variable to hold randomly generated number.
    int num;
    
    // Generating a random number (0-9)
    srand(static_cast<unsigned int>(time(0)));
    num = rand()%10;
    
    m_key = (randomStringGenerator(5) + randomStringGenerator(5) + (to_string(num)) + randomStringGenerator(5));
    

}

/**
* Encrypter::vigenerecipher. Vigenere cipher method.
* Method to encrypt message using vigenere's cipher.
* @param a_message string Target message to encrypt.
* @param b_key int index that key starts from.
* @param e_key int index that the key ends.
* @return string Vigenere encrypted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Encrypter::vigenereCipher(string a_message, int b_key, int e_key)
{
    // Variable to hold encrypted message
    string encrypt="";
    
    // String iterator
    int i = 0, j;
    int temp;
    
    // Vigenere encryption:
    while(i<a_message.length())
    {
        for (j = b_key; j < e_key; j++, i++)
        {
            // Adding the ascii values.
            // Mod for values that go out of printable ASCII table.
            // Checking if we have length (!mod5), we don't have to continue encrypting.
            if(i<a_message.length()){
                temp = ((a_message[i]-0)+(m_key[j]-0));
                
                while(temp > 126)
                    temp = (temp - 127) + 32;

                // Recording character
                encrypt += (char)temp;
            }
        }
    }
    return encrypt;
}

/**
* Encrypter::sBoxSubstitution. S-box subtitution method.
* Character substitution using the S-box.
* @param a_message string Target message to substitute using s-box.
* @return string S-box substituted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Encrypter::sBoxSubstitution(string a_message)
{
    // String iterator
    int i;
    // New substituted string holder
    string s_string = "";
    
    // Printable ASCII character to S-BOX index map.
    for (i=0; i<a_message.size(); i++)
        s_string += (char)m_s_box[((a_message[i]-0)-32)];

    return s_string;
}

/**
* Encrypter::pboxPermutaation. P-box permutation method.
* Character permutation using the p-box.
* @param a_message string Target message to permute using p-box.
* @return string P-box permuted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Encrypter::pboxPermutaation(string a_message)
{
    // Permuted string
    string p_string="";
    string temp;
    // Substr:
    string subst;
    
    // iterator
    int i;
    
    // Checking if length is %5, if not we add 'n' placeholders of char '|'
    if((a_message.size()%5) != 0)
    {
        int mod = a_message.size()%5;
        mod = 5-mod;
        
        while(mod != 0){
            a_message += "|";
            mod--;
        }
    }

    // Permutation of every 5 characters
    for(i = 0; i < a_message.size(); i+=5)
    {
        // Getting 5 characters every iteration:
        subst = a_message.substr(i,5);
        // Initializing temp
        temp = "";
        temp = (std::string() + subst[2] + subst[4]+subst[1] +subst[0]+subst[3]);
        p_string += temp;
    }
    return p_string;
}

/**
* Encrypter::caesarCipher. Caesar cipher encryption method.
*  Method that encrypts text using caesar's cipher.
* @param a_message string Target message to encrypt.
* @return string Encrypted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Encrypter::caesarCipher(string a_message)
{
    // The key used for Caesar encryption is on the 11th position of the string.
    int key = (int)m_key[10];
    
    // Variable to hold encrypted message
    string encrypt="";
    
    // String iterator
    int i = 0;
    int temp;
    
    // Caesar encryption:
    while(i<a_message.length())
    {
        // Adding num to the ascii values.
        // Mod for values that go out of printable ASCII table.
        // Checking if we have length (!mod5), we don't have to continue encrypting.
        temp = ((a_message[i]-0)+ key);
        if(temp > 126)
            temp = (temp - 127) + 32;

        // Recording character
        encrypt += (char)temp;
        // Moving to the next character
        i++;
    }
    return encrypt;
}

/**
* Encrypter::fiveBlockFeistel. 5 block Feistel style cipher.
*  Method that encrypts text using 5 block Feistel style cipher.
* @param a_message string Target message to encrypt.
* @return vector<bitset<8>> Encrypted message in binary in a vector.
* @author Salil Maharjan
* @date 10/10/19.
*/
vector<bitset<8>> Encrypter::fiveBlockFeistel(string a_message)
{
    // Vector of bitset to store the message in bits.
    // One character in each bitset.
    vector<bitset<8>> vecMsgBits;
    // Vector of bitset to store the key in bits.
    vector<bitset<8>> vecKeyBits;
       
    // iterator variable:
    int i,j;
    
    // Create vector of message in bits.
    for(i=0; i<a_message.size(); i++)
    {
        bitset<8> tmpMsg(a_message[i]);
        vecMsgBits.push_back(tmpMsg);
    }
      
    // Checking if length is %5, if not we add 'n' placeholders of bitset with 0's
    if((a_message.size() % 5) != 0)
    {
        int mod = a_message.size() % 5;
        mod = 5-mod;
        while(mod != 0){
            bitset<8> tmpNulls (0);
            vecMsgBits.push_back(tmpNulls);
            mod--;
        }
    }
    
    // Creating vector of key in bits
    for(i=0; i<m_key.size(); i++)
    {
        bitset<8> tmpKey(m_key[i]);
        vecKeyBits.push_back(tmpKey);
    }

    // Temp bitset vector to hold the new encrypted vector.
    vector<bitset<8>> tmp = vecMsgBits;
        
    // PERMUTATION and XOR in each round:
    // pos(0) = ^pos(2)
    // pos(1) = ^pos(4)
    // pos(2) = pos(3)
    // pos(3) = ^pos(0)
    // pos(4) = pos(1)
    // Loop to use each bit for encryption. - 16 rounds.
    for(j=0;j<vecKeyBits.size();j++)
    {
        // Loop to go through the entire message
        for(i=0; i<vecMsgBits.size();i+=5)
        {
            // Permutation and XOR
            tmp[i] = (vecMsgBits[i+2] ^ vecKeyBits[j]);
            tmp[i+1] = (vecMsgBits[i+4] ^ vecKeyBits[j]);
            tmp[i+2] = vecMsgBits[i+3];
            tmp[i+3] = (vecMsgBits[i] ^ vecKeyBits[j]);
            tmp[i+4] = vecMsgBits[i+1];
        }
        vecMsgBits = tmp;
    }
    vecMsgBits = tmp;
    
//    // Turning the bits into a string and returning it:
//    string encrypt = "";
//    for(i=0; i<vecMsgBits.size(); i++)
//    {
//        encrypt += (char)vecMsgBits[i].to_ulong();
//    }
    
    return vecMsgBits;
}
