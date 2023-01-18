/* 
 * final_luudanny.c
 * SHA-1
 * Danny Luu
 * 03/31/2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define MAX_SIZE 1048576

const bool debug = false;
unsigned int sha1_h[5] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0};
const unsigned int sha1_k[4] = {
    0x5A827999,
    0x6ED9EBA1,
    0x8F1BBCDC,
    0xCA62C1D6};

unsigned int readFile(unsigned char buffer[]);
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes);
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes);
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount);
void computeMessageDigest(unsigned int message[], unsigned int blockCount);
unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D);
unsigned int k(unsigned int t);

int main(void){

    unsigned char buffer[MAX_SIZE];
    unsigned int message[MAX_SIZE / 4] = {0};
    unsigned int msgLen = 0;
    unsigned int blockCount = 0;

    msgLen = readFile(buffer);
    blockCount = calculateBlocks(msgLen);
    convertCharArrayToIntArray(buffer, message, msgLen);
    addBitCountToLastBlock(message, msgLen, blockCount);
    computeMessageDigest(message, blockCount);
    return 0;
}

/*
 * Reads file contents and stores into a buffer[] array
 * @param buffer[] : The storage location of the file contents
 * @return unsigned int : The count of characters
 */
unsigned int readFile(unsigned char buffer[]){
    
    int i = 0;
    int count = 0;
    char letter = 'a';

    while ((letter = fgetc(stdin)) != EOF){
        
        // Add character to buffer
        buffer[count] = letter;
        
        // If file is too large for buffer[]
        if (count > MAX_SIZE){
            puts("ERROR: FILE is too large for the program!");
            exit(1);
        }   
        
        count ++;
    }
    
    // Append the bit '1' to buffer
    buffer[count] = 0x80;

    // Print DEBUG
    if (debug){
        
        puts("\nDisplay All Character In File");
        for (i = 0; i <= count; i++){
            printf("Buffer[%d] = '%c' = 0x%X\n", i, buffer[i], buffer[i]);
        }
        printf("\nCharacter Count = %d\n", count);
    }

    return count;
}

/*
 * Calculates the required number of blocks
 * @param sizeOfFileInBytes : Size of the file
 * @return unsigned int : Block count
 */
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes){
    
    int blockCount = 0;

    // Calculates block count
    blockCount = (((8 * sizeOfFileInBytes) + 1) / 512) + 1;
    
    // Checks if exta block is needed
    if ((((8 * sizeOfFileInBytes) + 1) % 512) > (512 - 64)){
        blockCount = blockCount + 1;
    }

    // Print DEBUG
    if (debug){
        printf("\nBlock Count = %d\n", blockCount);
    }

    return blockCount;
}

/*
 * Converts an unsigned char [] to an unsigned int [] by merging 4 char into 1 int.
 * @param buffer[] : The stored file in char form
 * @param message[] : The stored file in int form
 * @param sizeOfFileInBytes : Size of the file
 */
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes){

    unsigned char a;
    unsigned int merge = 0;
    int i = 0;
    int index = 0;

    // Merges 4 unsigned char into a single unsigned int; Then adds to message[]
    while (i <= sizeOfFileInBytes){
        
        switch(i % 4){
            case 0:
                a = buffer[i];
                merge = merge | (a << 24);
                message[index] = merge;
                break;
            case 1:
                a = buffer[i];
                merge = merge | (a << 16);
                message[index] = merge;
                break;
            case 2:
                a = buffer[i];
                merge = merge | (a << 8);
                message[index] = merge;
                break;
            case 3:
                a = buffer[i];
                merge = merge | (a);
                message[index] = merge;
                merge = 0;
                index++;
                break;
            default:
                break;
        }
        i++;
    }
    
    // Print DEBUG
    if (debug){       
        puts("\nDisplay Characters In 16 Integer (WORD) Form:");
        for(i = 0; i < (((index / 16) + 1) * 16); i += 4){
            
            if (i == 16){
                puts("");
            }

            printf("%-11.8X %-11.8X %-11.8X %-11.8X\n", message[i], message[i + 1], message[i + 2], message[i + 3]);
        }
    }
}

/*
 * Appends the size of file in the last index of the last block
 * @param message[] : The stored file in int form
 * @param sizeOfFileInBytes : Size of the file
 * @param blockCount : Block count
 */
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount){
    
    unsigned int sizeOfFileInBits = sizeOfFileInBytes * 8;
    unsigned int indexOfEndOfLastBlock = (blockCount * 16) - 1;
    int i = 0;

    message[indexOfEndOfLastBlock] = sizeOfFileInBits;

    // Print DEBUG
    if (debug){
        puts("\nDisplay the blocks:");       
        for (i = 0; i < (blockCount * 16); i += 4){

            if(i == 16){
                puts("");
            }

            printf("%-11.8X %-11.8X %-11.8X %-11.8X\n", message[i], message[i + 1], message[i + 2], message[i + 3]);
        }
    }
}

/*
 * Calculates the message digest (hash value) of the given file
 * @param message[] : The stored file in int form
 * @param blockCount : Block Count
 */
void computeMessageDigest(unsigned int message[], unsigned int blockCount){
    
    unsigned int A, B, C, D, E, temp;
    unsigned int H0 = sha1_h[0];
    unsigned int H1 = sha1_h[1];
    unsigned int H2 = sha1_h[2];
    unsigned int H3 = sha1_h[3];
    unsigned int H4 = sha1_h[4];
    unsigned int w[80];
    int count = 0;
    int index = 0;
    int t = 0;

    // Digest 1 block at a time 
    while (count < blockCount){
        count++;
        A = H0;
        B = H1;
        C = H2;
        D = H3;
        E = H4;

        // 'index' Keeps track of the "WORD" index in the message[]
        for (t = 0; t <= 15; t++){
            w[t] = message[index];
            index++;
        }
        for (t = 16; t <= 79; t++){
            w[t] = (w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16]);
            w[t] = ((w[t] << 1) | (w[t] >> 31));
        }

        // Main Digest Loop
        for (t = 0; t <= 79; t++){
            temp = ((A << 5) | (A >> 27)) + f(t, B, C, D) + E + w[t] + k(t);
            E = D;
            D = C;
            C = ((B << 30) | (B >> 2));
            B = A;
            A = temp;
            
            // Print DEBUG
            if (debug){
                if (t == 0){
                    printf("\n%16c %11c %11c %11c %11c\n", 'A', 'B', 'C', 'D', 'E');
                }
                printf("t = %2d: %11.8X %11.8X %11.8X %11.8X %11.8X\n", t, A, B, C, D, E);
            }
        }
    
        // Print DEBUG
        if (debug){
            printf("\nBlock %d has been proceed. The values of {H} are:\n", count);
            printf("H0 = %.8X + %.8X = %.8X\n", H0, A, H0 + A);
            printf("H1 = %.8X + %.8X = %.8X\n", H1, B, H1 + B);
            printf("H2 = %.8X + %.8X = %.8X\n", H2, C, H2 + C);
            printf("H3 = %.8X + %.8X = %.8X\n", H3, D, H3 + D);
            printf("H4 = %.8X + %.8X = %.8X\n", H4, E, H4 + E);
        }
        
        // Message digest for current block count
        H0 = H0 + A;
        H1 = H1 + B;
        H2 = H2 + C;
        H3 = H3 + D;
        H4 = H4 + E;
    } // Close Block Loop

    // Message Digest
    printf("\nMessage Digest (Hash Value) = %.8X %.8X %.8X %.8X %.8X\n", H0, H1, H2, H3, H4);
}


/*
 * Help Function : f
 * @param t : Determines which function to use
 * @param B C D : The unsigned ints used for calculation
 * @param unsigned int : Output
 */
unsigned int f(unsigned int t, unsigned int B, unsigned C, unsigned D){

    unsigned int output = 0;

    if ((0 <= t) && (t <= 19)){
        output = (B & C) | ((~B) & D);
    }
    else if ((20 <= t) && (t <= 39)){
        output = (B ^ C ^ D);
    }
    else if ((40 <= t) && (t <= 59)){
        output = (B & C) | (B & D) | (C & D);
    }
    else if ((60 <= t) && (t <= 79)){
        output = (B ^ C ^ D);
    }

    return output;
}

/* 
 * Helper Function : k
 * @param t : Determines which k value to return
 * @return unsigned int : The appropriate k value
 */
unsigned int k(unsigned int t){
   
    if ((0 <= t) && (t <= 19)){
        return sha1_k[0];    
    }
    else if ((20 <= t) && (t <= 39)){
        return sha1_k[1];
    }
    else if ((40 <= t) && (t <= 59)){
        return sha1_k[2];
    }
    else {
        return sha1_k[3];
    }
}
