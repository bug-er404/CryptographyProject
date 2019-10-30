/**
*  main.cpp
*  Main for the crypotography project.
*  Defines the entry point.
*
*
*  Created by Salil D. Maharjan on 10/10/19.
*  Copyright © 2019 Salil Maharjan. All rights reserved.
*/

#include "PrefixHeader.pch"
#include "Encrypter.hpp"
#include "Decrypter.hpp"


int main(int argc, char *argv[])
{
    // Checking for correct number of arguments
//    if (argc!=2)
//    {
//        cerr<<"Usage: DuckInterp <filename>"<<endl;
//        return 1;
//    }
    
    // To clock the time taken for encyrption
    clock_t m_start, m_end;
    
    // Create Encrypter object and send file to encrypt:
    Encrypter encrypt;
    m_start = clock();
//    encrypt.EncryptFile(argv[1]);
    encrypt.EncryptFile("message.txt");
    m_end = clock();
    
    cout<<"Encryption time: " << ((double(m_end - m_start)/CLOCKS_PER_SEC) *1000) <<endl;
    
    // For Decryption:
    Decrypter decrypt;
    decrypt.DecryptMessage();
    
    return 0;
}
