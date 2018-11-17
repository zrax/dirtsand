/******************************************************************************
 * This file is part of dirtsand.                                             *
 *                                                                            *
 * dirtsand is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU Affero General Public License as             *
 * published by the Free Software Foundation, either version 3 of the         *
 * License, or (at your option) any later version.                            *
 *                                                                            *
 * dirtsand is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Affero General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the GNU Affero General Public License   *
 * along with dirtsand.  If not, see <http://www.gnu.org/licenses/>.          *
 ******************************************************************************/

#include "CryptIO.h"
#include "errors.h"
#include <botan/auto_rng.h>
#include <botan/bigint.h>
#include <botan/pow_mod.h>
#include <botan/numthry.h>
#include <botan/stream_cipher.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
#include <mutex>
#include <memory>

#ifdef DEBUG
bool s_commdebug = false;
std::mutex s_commdebug_mutex;
#endif

void DS::GenPrimeKeys(Botan::BigInt& K, Botan::BigInt& N)
{
    Botan::AutoSeeded_RNG rng;
    DS_ASSERT(rng.is_seeded());

    K = Botan::BigInt();
    while (K.bytes() != 64) {
        K = Botan::random_safe_prime(rng, 512);
        putc('.', stdout);
        fflush(stdout);
    }

    N = Botan::BigInt();
    while (N.bytes() != 64) {
        N = Botan::random_safe_prime(rng, 512);
        putc('.', stdout);
        fflush(stdout);
    }
}

Botan::BigInt DS::CryptCalcX(const Botan::BigInt& N, const Botan::BigInt& K,
                             uint32_t base)
{
    /* X = base ^ K % N */
    Botan::Power_Mod pow_mod(N, Botan::Power_Mod::BASE_IS_SMALL);
    pow_mod.set_base(base);
    pow_mod.set_exponent(K);
    return pow_mod.execute();
}

void DS::CryptEstablish(uint8_t* seed, uint8_t* key, const Botan::BigInt& N,
                        const Botan::BigInt& K, const Botan::BigInt& Y)
{
    Botan::AutoSeeded_RNG rng;
    DS_ASSERT(rng.is_seeded());

    /* Random 7-byte server seed */
    rng.randomize(seed, 7);

    /* client = Y ^ K % N */
    DS_ASSERT(!N.is_zero());
    Botan::Power_Mod pow_mod(N);
    pow_mod.set_base(Y);
    pow_mod.set_exponent(K);
    auto bn_seed = pow_mod.execute();

    /* Apply server seed for establishing crypt state with client */
    uint8_t keybuf[64];
    if (bn_seed.bytes() > sizeof(keybuf))
        throw DS::InvalidConnectionHeader();
    bn_seed.binary_encode(keybuf);
    BYTE_SWAP_BUFFER(keybuf, bn_seed.bytes());
    for (size_t i = 0; i < 7; ++i)
        key[i] = keybuf[i] ^ seed[i];
}


struct CryptState_Private
{
    std::unique_ptr<Botan::StreamCipher> m_readCipher;
    std::unique_ptr<Botan::StreamCipher> m_writeCipher;
};

DS::CryptState DS::CryptStateInit(const uint8_t* key, size_t size)
{
    CryptState_Private* state = new CryptState_Private;
    state->m_readCipher = Botan::StreamCipher::create("RC4");
    state->m_writeCipher = Botan::StreamCipher::create("RC4");
    state->m_readCipher->set_key(key, size);
    state->m_writeCipher->set_key(key, size);
    return reinterpret_cast<CryptState>(state);
}

void DS::CryptStateFree(DS::CryptState state)
{
    if (!state)
        return;

    CryptState_Private* statep = reinterpret_cast<CryptState_Private*>(state);
    delete statep;
}

void DS::CryptSendBuffer(const DS::SocketHandle sock, DS::CryptState crypt,
                         const void* buffer, size_t size)
{
#ifdef DEBUG
    if (s_commdebug) {
        std::lock_guard<std::mutex> commdebugGuard(s_commdebug_mutex);
        printf("SEND TO %s", DS::SockIpAddress(sock).c_str());
        for (size_t i=0; i<size; ++i) {
            if ((i % 16) == 0)
                fputs("\n    ", stdout);
            else if ((i % 16) == 8)
                fputs("   ", stdout);
            printf("%02X ", reinterpret_cast<const uint8_t*>(buffer)[i]);
        }
        fputc('\n', stdout);
    }
#endif

    CryptState_Private* statep = reinterpret_cast<CryptState_Private*>(crypt);
    if (!statep) {
        DS::SendBuffer(sock, buffer, size);
    } else if (size > 4096) {
        std::unique_ptr<uint8_t[]> cryptbuf(new uint8_t[size]);
        statep->m_writeCipher->cipher(reinterpret_cast<const uint8_t*>(buffer),
                                      cryptbuf.get(), size);
        DS::SendBuffer(sock, cryptbuf.get(), size);
    } else {
        uint8_t stack[4096];
        statep->m_writeCipher->cipher(reinterpret_cast<const uint8_t*>(buffer),
                                      stack, size);
        DS::SendBuffer(sock, stack, size);
    }
}

void DS::CryptRecvBuffer(const DS::SocketHandle sock, DS::CryptState crypt,
                         void* buffer, size_t size)
{
    CryptState_Private* statep = reinterpret_cast<CryptState_Private*>(crypt);
    if (!statep) {
        DS::RecvBuffer(sock, buffer, size);
    } else if (size > 4096) {
        std::unique_ptr<uint8_t[]> cryptbuf(new uint8_t[size]);
        DS::RecvBuffer(sock, cryptbuf.get(), size);
        statep->m_readCipher->cipher(cryptbuf.get(), reinterpret_cast<uint8_t*>(buffer), size);
    } else {
        uint8_t stack[4096];
        DS::RecvBuffer(sock, stack, size);
        statep->m_readCipher->cipher(stack, reinterpret_cast<uint8_t*>(buffer), size);
    }

#ifdef DEBUG
    if (s_commdebug) {
        std::lock_guard<std::mutex> commdebugGuard(s_commdebug_mutex);
        printf("RECV FROM %s", DS::SockIpAddress(sock).c_str());
        for (size_t i=0; i<size; ++i) {
            if ((i % 16) == 0)
                fputs("\n    ", stdout);
            else if ((i % 16) == 8)
                fputs("   ", stdout);
            printf("%02X ", reinterpret_cast<const uint8_t*>(buffer)[i]);
        }
        fputc('\n', stdout);
    }
#endif
}

ST::string DS::CryptRecvString(const SocketHandle sock, CryptState crypt)
{
    uint16_t length = CryptRecvValue<uint16_t>(sock, crypt);
    ST::utf16_buffer result;
    char16_t* buffer = result.create_writable_buffer(length);
    CryptRecvBuffer(sock, crypt, buffer, length * sizeof(char16_t));
    buffer[length] = 0;
    return ST::string::from_utf16(result, ST::substitute_invalid);
}

DS::ShaHash DS::BuggyHashPassword(const ST::string& username, const ST::string& password)
{
    ST::utf16_buffer wuser = username.to_utf16();
    ST::utf16_buffer wpass = password.to_utf16();
    ST::utf16_buffer work;
    char16_t* buffer = work.create_writable_buffer(wuser.size() + wpass.size());
    memcpy(buffer, wpass.data(), wpass.size() * sizeof(char16_t));
    memcpy(buffer + wpass.size(), wuser.data(), wuser.size() * sizeof(char16_t));
    buffer[wpass.size() - 1] = 0;
    buffer[wpass.size() + wuser.size() - 1] = 0;
    return ShaHash::Sha0(buffer, (wuser.size() + wpass.size()) * sizeof(char16_t));
}

DS::ShaHash DS::BuggyHashLogin(const ShaHash& passwordHash, uint32_t serverChallenge,
                               uint32_t clientChallenge)
{
    struct
    {
        uint32_t m_clientChallenge, m_serverChallenge;
        ShaHash m_pwhash;
    } buffer;

    buffer.m_clientChallenge = clientChallenge;
    buffer.m_serverChallenge = serverChallenge;
    buffer.m_pwhash = passwordHash;
    return ShaHash::Sha0(&buffer, sizeof(buffer));
}
