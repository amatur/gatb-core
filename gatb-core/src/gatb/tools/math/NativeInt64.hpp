/*****************************************************************************
 *   GATB : Genome Assembly Tool Box                                         *
 *   Authors: [R.Chikhi, G.Rizk, E.Drezen]                                   *
 *   Based on Minia, Authors: [R.Chikhi, G.Rizk], CeCILL license             *
 *   Copyright (c) INRIA, CeCILL license, 2013                               *
 *****************************************************************************/

/** \file NativeInt64.hpp
 *  \date 01/03/2013
 *  \author edrezen
 *  \brief Integer class relying on native u_int64_t type
 */

#ifndef _GATB_CORE_TOOLS_MATH_INTEGER_NATIVE_64_HPP_
#define _GATB_CORE_TOOLS_MATH_INTEGER_NATIVE_64_HPP_

/********************************************************************************/

#include <iostream>
#include <gatb/system/api/types.hpp>
#include <hdf5.h>

extern const unsigned char revcomp_4NT[];
extern const unsigned char comp_NT    [];
extern const u_int64_t random_values    [256];

/********************************************************************************/
namespace gatb  {
namespace core  {
namespace tools {
/** \brief Math package */
namespace math  {
/********************************************************************************/

/** \brief Large integer class
 */
class NativeInt64 : private ArrayData<u_int64_t, 1>
{
public:

    /** Constructor.
     * \param[in] c : initial value of the large integer. */
    NativeInt64 (const u_int64_t& c=0)  {  value[0] = c;  }

    static const char* getName ()  { return "NativeInt64"; }

    static const size_t getSize ()  { return 8*sizeof(u_int64_t); }

    NativeInt64 operator+  (const NativeInt64& other)   const   {  return value[0] + other.value[0];  }
    NativeInt64 operator-  (const NativeInt64& other)   const   {  return value[0] - other.value[0];  }
    NativeInt64 operator|  (const NativeInt64& other)   const   {  return value[0] | other.value[0];  }
    NativeInt64 operator*  (const int& coeff)           const   {  return value[0] * coeff;        }
    NativeInt64 operator/  (const u_int32_t& divisor)   const   {  return value[0] / divisor;      }
    u_int32_t   operator%  (const u_int32_t& divisor)   const   {  return value[0] % divisor;      }
    NativeInt64 operator^  (const NativeInt64& other)   const   {  return value[0] ^ other.value[0];  }
    NativeInt64 operator&  (const NativeInt64& other)   const   {  return value[0] & other.value[0];  }
    NativeInt64 operator&  (const char& other)          const   {  return value[0] & other;        }
    NativeInt64 operator~  ()                           const   {  return ~value[0];               }
    NativeInt64 operator<< (const int& coeff)           const   {  return value[0] << coeff;       }
    NativeInt64 operator>> (const int& coeff)           const   {  return value[0] >> coeff;       }
    bool        operator!= (const NativeInt64& c)       const   {  return value[0] != c.value[0];     }
    bool        operator== (const NativeInt64& c)       const   {  return value[0] == c.value[0];     }
    bool        operator<  (const NativeInt64& c)       const   {  return value[0] < c.value[0];      }
    bool        operator<= (const NativeInt64& c)       const   {  return value[0] <= c.value[0];     }

    NativeInt64& operator+=  (const NativeInt64& other)    {  value[0] += other.value[0]; return *this; }
    NativeInt64& operator^=  (const NativeInt64& other)    {  value[0] ^= other.value[0]; return *this; }

    u_int8_t  operator[]  (size_t idx)    {  return (value[0] >> (2*idx)) & 3; }

    /********************************************************************************/
    friend std::ostream & operator<<(std::ostream & s, const NativeInt64 & l)
    {
        s << std::hex << l.value[0] << std::dec;  return s;
    }
    /********************************************************************************/
    /** Print corresponding kmer in ASCII
     * \param[sizeKmer] in : kmer size (def=32).
     */
    inline void printASCII ( size_t sizeKmer = 32)
    {
        int i;
        u_int64_t temp = value[0];

        
        char seq[33];
        char bin2NT[4] = {'A','C','T','G'};
        
        for (i=sizeKmer-1; i>=0; i--)
        {
            seq[i] = bin2NT[ temp&3 ];
            temp = temp>>2;
        }
            seq[sizeKmer]='\0';
        
        std::cout << seq << std::endl;
    }

    /********************************************************************************/
    /** Print corresponding kmer in ASCII
     * \param[sizeKmer] in : kmer size (def=32).
     */
    std::string toString (size_t sizeKmer) const
    {
        int i;
        u_int64_t temp = value[0];

        char seq[33];
        char bin2NT[4] = {'A','C','T','G'};

        for (i=sizeKmer-1; i>=0; i--)
        {
            seq[i] = bin2NT[ temp&3 ];
            temp = temp>>2;
        }
        seq[sizeKmer]='\0';
        return seq;
    }

    
    /********************************************************************************/
    inline static u_int64_t revcomp64 (const u_int64_t& x, size_t sizeKmer)
    {
        u_int64_t res = x;

        unsigned char* kmerrev  = (unsigned char *) (&(res));
        unsigned char* kmer     = (unsigned char *) (&(x));

        for (size_t i=0; i<8; ++i)  {  kmerrev[8-1-i] = revcomp_4NT [kmer[i]];  }

        return (res >> (2*( 32 - sizeKmer))) ;
    }

    /********************************************************************************/
    inline static u_int64_t hash64 (u_int64_t key, u_int64_t seed)
    {
        u_int64_t hash = seed;
        hash ^= (hash <<  7) ^  key * (hash >> 3) ^ (~((hash << 11) + (key ^ (hash >> 5))));
        hash = (~hash) + (hash << 21); // hash = (hash << 21) - hash - 1;
        hash = hash ^ (hash >> 24);
        hash = (hash + (hash << 3)) + (hash << 8); // hash * 265
        hash = hash ^ (hash >> 14);
        hash = (hash + (hash << 2)) + (hash << 4); // hash * 21
        hash = hash ^ (hash >> 28);
        hash = hash + (hash << 31);
        return hash;
    }

    /********************************************************************************/
    inline static u_int64_t oahash64 (u_int64_t elem)
    {
        u_int64_t code = elem;
        code = code ^ (code >> 14); //supp
        code = (~code) + (code << 18);
        code = code ^ (code >> 31);
        code = code * 21;
        code = code ^ (code >> 11);
        code = code + (code << 6);
        code = code ^ (code >> 22);
        return code;
    }

    /********************************************************************************/
    /** computes a simple, naive hash using only 16 bits from input key
     * \param[shift] in : selects which of the input byte will be used for hash computation
     */
    inline static  u_int64_t    simplehash16_64   (u_int64_t key, int  shift)
    {
        u_int64_t input = key >> shift;
        u_int64_t res = random_values[input & 255]   ;
        
        input = input  >> 8;
        res  ^= random_values[input & 255] ;
        
        return res;
        //could be improved by xor'ing result of multiple bytes
    }

    /********************************************************************************/
    inline static hid_t hdf5 (bool& isCompound)
    {
        return H5Tcopy (H5T_NATIVE_UINT64);
    }

private:

    friend NativeInt64 revcomp (const NativeInt64& i,   size_t sizeKmer);
    friend u_int64_t    hash1    (const NativeInt64& key, u_int64_t  seed);
    friend u_int64_t    oahash  (const NativeInt64& key);
    friend u_int64_t    simplehash16    (const NativeInt64& key, int  shift);

};

/********************************************************************************/
inline NativeInt64 revcomp (const NativeInt64& x, size_t sizeKmer)
{
    return NativeInt64::revcomp64 (x.value[0], sizeKmer);
}

/********************************************************************************/
inline u_int64_t hash1 (const NativeInt64& key, u_int64_t seed=0)
{
    return NativeInt64::hash64 (key.value[0], seed);
}

/********************************************************************************/
inline u_int64_t oahash (const NativeInt64& key)
{
    return NativeInt64::oahash64 (key.value[0]);
}
    
/********************************************************************************/
inline u_int64_t simplehash16 (const NativeInt64& key, int  shift)
{
    return NativeInt64::simplehash16_64 (key.value[0], shift);
}

/********************************************************************************/
} } } } /* end of namespaces. */
/********************************************************************************/

#endif /* _GATB_CORE_TOOLS_MATH_INTEGER_NATIVE_64_HPP_ */
