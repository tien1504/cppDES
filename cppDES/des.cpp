#include "des.h"
#include "des_data.h"

DES::DES(ui64 key)
{
    keygen(key);
}

ui64 DES::encrypt(ui64 block)
{
    return des(block, false);
}

ui64 DES::decrypt(ui64 block)
{
    return des(block, true);
}

ui64 DES::encrypt(ui64 block, ui64 key)
{
    DES des(key);
    return des.des(block, false);
}

ui64 DES::decrypt(ui64 block, ui64 key)
{
    DES des(key);
    return des.des(block, true);
}

void DES::keygen(ui64 key)
{
    // initial key schedule calculation
    ui64 permuted_choice_1 = 0; // 56 bits
    for (ui8 i = 0; i < 56; i++)
    {
        permuted_choice_1 <<= 1;
        permuted_choice_1 |= (key >> (64-PC1[i])) & LB64_MASK;
    }

    // 28 bits
    ui32 C = (ui32) ((permuted_choice_1 >> 28) & 0x000000000fffffff);
    ui32 D = (ui32) (permuted_choice_1 & 0x000000000fffffff);

    // Calculation of the 16 keys
    for (ui8 i = 0; i < 16; i++)
    {
        // key schedule
        // shifting Ci and Di
        for (ui8 j = 0; j < iteration_shift[i]; j++)
        {
            C = (0x0fffffff & (C << 1)) | (0x00000001 & (C >> 27));
            D = (0x0fffffff & (D << 1)) | (0x00000001 & (D >> 27));
        }

        ui64 permuted_choice_2 = (((ui64) C) << 28) | (ui64) D ;

        sub_key[i] = 0;
        for (ui8 j = 0; j < 48; j++)
        {
            sub_key[i] <<= 1;
            sub_key[i] |= (permuted_choice_2 >> (56-PC2[j])) & LB64_MASK;
        }
    }
}

ui64 DES::des(ui64 block, bool mode)
{
    block = ip(block);

    ui32 L = (ui32) (block >> 32) & L64_MASK;;
    ui32 R = (ui32) (block & L64_MASK);

    for (ui8 i = 0; i < 16; i++)
    {
        ui32 F = mode ? f(R, sub_key[15-i]) : f(R, sub_key[i]);
        feistel(L, R, F);
    }

    block = (((ui64) R) << 32) | (ui64) L;
    block = pi(block);
    return block;
}

ui64 DES::ip(ui64 block)
{
    // initial permutation
    ui64 result = 0;
    for (ui8 i = 0; i < 64; i++)
    {
        result <<= 1;
        result |= (block >> (64-IP[i])) & LB64_MASK;
    }
    return result;
}

ui64 DES::pi(ui64 block)
{
    // inverse initial permutation
    ui64 result = 0;
    for (ui8 i = 0; i < 64; i++)
    {
        result <<= 1;
        result |= (block >> (64-PI[i])) & LB64_MASK;
    }
    return result;
}

void DES::feistel(ui32 &L, ui32 &R, ui32 F)
{
    ui32 temp = R;
    R = L ^ F;
    L = temp;
}

ui32 DES::f(ui32 R, ui64 k)
{
    // f(R,k) function
    ui64 s_input = 0;

    for (ui8 i = 0; i < 48; i++)
    {

        s_input <<= 1;
        s_input |= (ui64) ((R >> (32-E[i])) & LB32_MASK);

    }

    // Encryption/Decryption
    // XORing expanded Ri with Ki
    s_input = s_input ^ k;

    // S-Box Tables
    ui32 s_output;
    for (ui8 i = 0; i < 8; i++)
    {
        // 00 00 RCCC CR00 00 00 00 00 00 s_input
        // 00 00 1000 0100 00 00 00 00 00 row mask
        // 00 00 0111 1000 00 00 00 00 00 column mask

        char row;
        row = (char) ((s_input & (0x0000840000000000 >> 6*i)) >> (42-6*i));
        row = (row >> 4) | (row & 0x01);

        char column = (char) ((s_input & (0x0000780000000000 >> 6*i)) >> (43-6*i));

        s_output <<= 4;
        s_output |= (ui32) (S[i][16*row + column] & 0x0f);
    }

    ui32 result = 0;
    for (ui8 i = 0; i < 32; i++)
    {
        result <<= 1;
        result |= (s_output >> (32 - P[i])) & LB32_MASK;
    }

    return result;
}