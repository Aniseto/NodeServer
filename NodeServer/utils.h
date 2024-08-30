#pragma once



#include <botan/hash.h>
#include <botan/hex.h>
#include <string>
#include <iostream>




/* To-DO
    Crypto functions to generate a NOSO addreess.*/

std::string CalculateRIPEMD160_Botan(const std::string& input)
{
    std::unique_ptr<Botan::HashFunction> hash(Botan::HashFunction::create("RIPEMD-160"));
    hash->update(reinterpret_cast<const uint8_t*>(input.data()), input.size());
    Botan::secure_vector<uint8_t> digest = hash->final();  // Usar secure_vector en lugar de std::vector
    return Botan::hex_encode(digest);
}

