/**
*  Decrypter.cpp
*  Implementation of Decrypter.hpp
*
*
*  Created by Salil D. Maharjan on 10/10/19.
*  Copyright © 2019 Salil Maharjan. All rights reserved.
*/

#include "Decrypter.hpp"
#include "PrefixHeader.pch"
#include "Encrypter.hpp"

// PUBLIC METHODS:
// *********************************************************
// ************** Default Constructor **********************
// *********************************************************
Decrypter::Decrypter(){}
// *********************************************************
// ************** Default Destructor ***********************
// *********************************************************
Decrypter::~Decrypter(){}

/**
* Decrypter::DecryptMessage. Decrypter method.
* Method to run the decrypter on message.
* @author Salil Maharjan
* @date 10/10/19.
*/
void Decrypter::DecryptMessage()
{
    // string to hold decrypted message in each step
    string decrypt;
    
    // string to hold filename
    string filename;
    
    // Get message and key for decryption.
    cout<<"Please enter the .txt filename with the encrypted message: "<<endl;
    //Input validation of file name.
    do{cin>>filename;} while (filename.find(".txt") == string::npos);
    
    cout<<"----------------------------------------------"<<endl;
    cout<<"----------------DECRYPTION!-------------------"<<endl;
    cout<<"----------------------------------------------"<<endl<<endl<<endl;
    // Output formatting
    cout<<left;
    
    // To clock the time taken for encyrption and decryption
    clock_t m_start, m_end;
    
    // Read the encrypted message and the key.
    readMessageKey(filename);
//    cout<<setw(35)<<"ENCRYPTED TEXT: "<<m_encrypted<<endl<<endl;
            
    // Start timer once key is received:
    m_start = clock();
    
    // 5 Block Feistel style decryption:
    decrypt = fiveBlockFeistel(m_encrypted);
//        cout<<setw(35)<<"5B Feistel Decipher: "<<decrypt<<endl<<endl;
    
    // Decryption using Caesar's cipher
    decrypt = caesarDecrypter(decrypt);
//       cout<<setw(35)<<"Caesar Decipher: "<<decrypt<<endl<<endl;
    
    // Decrypting with vigenere cipher using byte 5-9 from the key.
    decrypt = vigenereDecrypter(decrypt, 5, 9);
//        cout<<setw(35)<<"Vigenere Decipher(3): "<<decrypt<<endl<<endl;
    
    // Decrypting using reverse P-BOX
    decrypt = revPBoxPermutation(decrypt);
//        cout<<setw(35)<<"Reverse P-BOX: "<<decrypt<<endl<<endl;
    
    // Decrypting with vigenere cipher using first 5 byte from the key.
    decrypt = vigenereDecrypter(decrypt, 0, 4);
//        cout<<setw(35)<<"Vigenere Decipher(2): "<<decrypt<<endl<<endl;
    
    // Reverse s-box substitution:
    decrypt = revSBoxSubstitution(decrypt);
//        cout<<setw(35)<<"REVERSE S-BOX: "<<decrypt<<endl<<endl;
    
    // Decrypting with vigenere cipher using last 5 byte from the key.
    decrypt = vigenereDecrypter(decrypt, 11, 16);
//        cout<<setw(35)<<"Vigenere Decipher(1): "<<decrypt<<endl<<endl;

    cout<<setw(35)<<"Decrypted message: "<<decrypt <<endl;
    
    // End clocking
    m_end = clock();
    
    cout<<"Decryption time: " << ((double(m_end - m_start)/CLOCKS_PER_SEC) *1000)<<endl;
}



// PRIVATE METHODS:
/**
* Decrypter::readMessageKey. Message and key reader method.
* Method to read the encrypted message and key and record it.
* @param a_fileName string Target file to get encrypted message from.
* @author Salil Maharjan
* @date 10/10/19.
*/
void Decrypter::readMessageKey(string a_fileName)
{
    // Temp variable to hold message
    string message = "";
    
    // Buffer to read the message
    string buffer;
    
    // Open file and check if it opened correctly.
    ifstream inf(a_fileName, ios::in | ios::binary);
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
    m_encrypted = message;
    
    cout<<"Please enter the key to decrypt the text: "<<endl;
    cin>> m_key;
    
}

/**
* Decrypter::vigenerecipher. Vigenere decrypter method.
* Method to decrypt message using vigenere's cipher.
* @param a_encrypted string Target message to decrypt.
* @param b_key int index that key starts from.
* @param e_key int index that the key ends.
* @return string Vigenere decrypted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Decrypter::vigenereDecrypter(string a_encrypted, int b_key, int e_key)
{
    // Variable to hold decrypted message
    string decrypt = "";
    // Temp variable to check for negative results.
    int temp;
    // String iterator
    int i = 0, j;
  
    // Vigenere decryption:
    while(i<a_encrypted.length())
    {
        for (j = b_key; j < e_key; j++, i++)
        {
            if(i<a_encrypted.length())
            {
                temp = ((a_encrypted[i]-0)-(m_key[j]-0));
                // Checking for negative values and control ascii characters:
                if(temp < 32)
                    // Maths to keep the range in printable ASCII.
                    temp = (127 -
                            ((m_key[j]-0)-
                             ((a_encrypted[i]-0)-32)));
                // If it is still out of range, make one more round.
                if(temp < 32) {
                    temp = 127 + (temp-32);
                }
                decrypt += temp;
            }
        }
    }
    return decrypt;
}

/**
* Decrypter::revSBoxSubstitution. S-box reverse subtitution method.
* Reverses character substitution using the S-box.
* @param a_encrypted string Target message to resubstitute using public s-box from Encrypter class.
* @return string S-box resubstituted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Decrypter::revSBoxSubstitution(string a_encrypted)
{
    // Encrypter object to use s-box
    Encrypter t_encrypt;
    
    // Iterators
    int i, j ;
    // New reverse substituted string holder
    string s_string = "";
    
    // Going through the encrypted text
    for (i=0; i<a_encrypted.size(); i++)
    {
        // Going through the s-box to find value corresponding encrypted value.
        for(j=0;j<95;j++)
        {
            if(t_encrypt.m_s_box[j] == (a_encrypted[i]-0))
                // ASCII Printable character range.
                s_string += (char)(j+32);
        }
    }
    
    return s_string;
}

/**
* Decrypter::revPBoxPermutation. P-box reverse permutation method.
* Reverses character permutation using the p-box.
* @param a_encrypted string Target message to repermute using public p-box.
* @return string S-box resubstituted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Decrypter::revPBoxPermutation(string a_encrypted)
{
    // Reverse permuted string
    string p_string="";
    string temp;
    // Substr:
    string subst;
    
    // iterator
    int i;
    // Checking if length is valid.
    if((a_encrypted.length()%5) == 0){
        // Reverse permutation of every 5 characters
        for(i = 0; i < a_encrypted.size(); i+=5)
        {
            // Getting 5 characters every iteration:
            subst = a_encrypted.substr(i,5);
            // Initializing temp
            temp = "";
            temp = (std::string() + subst[3] + subst[2]+subst[0] +subst[4]+subst[1]);
            p_string += temp;
        }
    }
    else{
        cerr<<"Invalid Encrypted Text! Please pass a valid text."<<endl;
        exit(1);
    }

    // Popping out the placeholder text
    while (p_string.back() == '|')
        p_string.pop_back();
    

    
    return p_string;
}

/**
* Decrypter::caesarDecrypter. Caesar decrypter method.
* Method to decrypt message using caesar's cipher.
* @param a_encrypted string Target message to decrypt.
* @return string  decrypted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Decrypter::caesarDecrypter(string a_encrypted)
{
    // The key used for Caesar encryption is on the 11th position of the string.
    int key = (int)m_key[10];
    
    // Variable to hold decrypted message
    string decrypt = "";
    
    // Temp variable to check for negative results.
    int temp;
    // String iterator
    int i = 0;
    
    // Caesar decryption:
    while(i<a_encrypted.length())
    {
        temp = ((a_encrypted[i]-0) - key);
        // Checking for negative values and control ascii characters:
        if(temp < 32)
            // Maths to keep the range in printable ASCII.
            temp = 127 + (temp-32);

        decrypt += temp;
        i++;
    }
    return decrypt;
}

/**
* Decrypter::fiveBlockFeistel. 5-Block Feistel style cipher decrypter method.
* Method to decrypt message by using the custom 5-Block Feistel style cipher
* @param a_encrypted string Target message to decrypt.
* @return string  decrypted message.
* @author Salil Maharjan
* @date 10/10/19.
*/
string Decrypter::fiveBlockFeistel(string a_encrypted)
{
    // Vector of bitset to store the encryption in bits.
    // One character in each bitset.
    vector<bitset<8>> vecMsgBits;
    // Vector of bitset to store the key in bits.
    vector<bitset<8>> vecKeyBits;
    
    // iterator variable:
    int i,j;
    string subst;
    
    // Create vector of message in bits.
    for(i=0; i<a_encrypted.size(); i+=8)
    {
        // Getting 8 characters (8 bit values) every iteration:
        subst = a_encrypted.substr(i,8);
        bitset<8> tmpMsg(subst);
        vecMsgBits.push_back(tmpMsg);
    }
   
    // Checking if length is %5, if not, it is not a valid encrypted text.
    if((a_encrypted.size() % 5) != 0)
    {
        cerr<<"Invalid Encrypted Text!(5BF) Please pass a valid text."<<endl;
        exit(1);
    }
        
    // Creating vector of key in bits
    for(i=0; i<m_key.size(); i++)
    {
        bitset<8> tmpKey(m_key[i]);
        vecKeyBits.push_back(tmpKey);
    }

    // temp bitset vector to hold the new decrypted vector.
    vector<bitset<8>> tmp = vecMsgBits;
    
    // PERMUTATION and XOR in each round:
    // pos(0) = ^pos(3)
    // pos(1) = pos(4)
    // pos(2) = ^pos(0)
    // pos(3) = pos(2)
    // pos(4) = ^pos(1)
     
    // Loop to use each bit for decryption. - 16 rounds.
    for(j=vecKeyBits.size()-1; j>=0; j--)
    {
        // Loop to go through the entire message
        for(i=0; i<vecMsgBits.size(); i+=5)
        {
            tmp[i] = (vecMsgBits[i+3] ^ vecKeyBits[j]);
            tmp[i+1] = vecMsgBits[i+4];
            tmp[i+2] = (vecMsgBits[i] ^ vecKeyBits[j]);
            tmp[i+3] = vecMsgBits[i+2];
            tmp[i+4] = (vecMsgBits[i+1] ^ vecKeyBits[j]);
        }
        vecMsgBits = tmp;
    }
    vecMsgBits = tmp;
    // Turn vector of bits to string and return it:
    string decrypt="";
    for(i=0 ; i<vecMsgBits.size(); i++)
    {
        decrypt += (char)vecMsgBits[i].to_ulong();
    }
    
    return decrypt;
}
