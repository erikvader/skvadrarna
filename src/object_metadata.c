#include "include/object_metadata.h"
#include "include/heap_metadata.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#define HEADER_SIZ 8
#define CHUNK_SIZ 2048
#define object_size_valid(X) (X<=(CHUNK_SIZ-HEADER_SIZ))

#define HEADER (object - HEADER_SIZ)

#define IS_FORWARDING (read_2_bits_last(object , 0)==1)
#define SET_FORWARDING {write_2_bits_last(object, 0, 1);}
#define IS_BIT_ARRAY (read_2_bits_last(object , 0)==3 && ( read_2_bits_last(object , 2)==0 || read_2_bits_last(object , 2)==2 ) )
#define SET_BIT_ARRAY {\
    write_2_bits_last(object, 0, 3);\
    write_2_bits_last(object, 2, 0);\
      }
#define IS_SIZE (read_2_bits_last(object , 0)==3 && ( read_2_bits_last(object , 2)==1 || read_2_bits_last(object , 2)==3) )
#define SET_SIZE {\
    write_2_bits_last(object, 0, 3);\
    write_2_bits_last(object, 2, 1);\
  }
#define IS_FORMAT_STRING (read_2_bits_last(object , 0)==0 || read_2_bits_last(object , 0)==2)
#define SET_FORMAT_STRING {write_2_bits_last(object, 0, 0);}

#define IS_EXPLORED ( read_2_bits_last(object , 0)==2 || ( read_2_bits_last(object , 0)==3 && (read_2_bits_last(object , 2) >= 2 )))
//-----------------------------------------HELPFUNCTIONS---------------------------





///Reads from (first + offset) 2 bits of header data
uint8_t read_2_bits_first(const void *object, const int offset) {
    uint64_t data = *(uint64_t *)(HEADER); // Data is a safe copy of header
    data = data >> (62 - offset);// 62 = bitsize of uint64_t - number of bits needed
    data = data & (uint64_t) 3;// Sets all but last 2 bits of data to 0;
    return data;
}
///Reads from (last - offset) 2 bits of header data
uint8_t read_2_bits_last(const void *object, const int offset) {
    return read_2_bits_first(object, HEADER_SIZ * 8 - (2 + offset));
}

///Writes to (first + offset) 2 bits of header data
void write_2_bits_first(void *object, const int offset, uint64_t data) {
    uint64_t *header_data = (uint64_t *)(HEADER); // header_data is the header
    data = data & (uint64_t) 3; // Sets all but last 2 bits of data to 0;
    data = data << (62 - offset); // 62 = bitsize of uint64_t - number of bits needed
    *header_data -= *header_data & ((uint64_t) 3 << (62 - offset)); // sets bites to write to to 0;
    *header_data = *header_data | data; // writes to bits
}

///Writes to (last - offset) 2 bits of header data
void write_2_bits_last(void *object, const int offset, uint64_t data) {
    write_2_bits_first(object, HEADER_SIZ * 8 - (2 + offset), data);
}

void *get_adress(const void *object) {
    uint16_t last_2 = read_2_bits_last(object, 0);
    return (*(void **)HEADER - last_2);
}

/// Magic function to make everything else easier
/// Writes bitrepresentation of or calculate the size of a format string.
///
/// \param object place where the object is stored
/// \param format format string to calculate size of and maybe write as bit array in the up to first 62 (including null termination) bits.
/// \param write If it should write or just calculate size.
/// \returns Size, unless if write is true and write failed due to not enough space. Then it returns 0.

size_t bit_array64(void *object, const char *format, bool write) {
    size_t size = 0;
    bool contains_8 = false;
    bool contains_4 = false;
    int no_bytes = 0;
    int no_8bytes = 0;
    uint8_t bit_array[strlen(format)];//break(00) = 0 , 8 bytes(01) = 1, 16 bytes (10) = 2, pointer (11) = 3;
    int bit_array_index = 0;
    while(*format != '\0') {
        switch(*format) {
        case 'b':
        case 'c':
            ++no_bytes;
            break;
        case 'i':
        case 'l':
        case 'f':
            contains_4 = true;
            if(no_bytes == 0) {
                no_bytes = 4;

            } else if(no_bytes <= 4) {
                no_bytes = 0;
                ++ no_8bytes;
            } else {
                no_bytes = 4;
                ++no_8bytes;
            }

            break;
        case 'd':
            contains_8 = true;
            if(no_bytes == 0) {
                ++no_8bytes;
            } else {
                no_8bytes += 2;
            }
            break;
        case '*':
            contains_8 = true;
            if(no_bytes > 0) {
                ++no_8bytes;
            }
            if(no_8bytes == 1) {
                bit_array[bit_array_index++] = 1;
                size += 8;
            } else if(no_8bytes == 2) {
                bit_array[bit_array_index++] = 2;
                size += 16;
            }
            no_bytes = 0;
            no_8bytes = 0;
            size += 8;
            bit_array[bit_array_index++] = 3;
            break;
        default:

            assert(false);//wrong format on format string;
            break;
        }

        if(no_bytes >= 8) {
            ++no_8bytes;
            no_bytes = 0;
        }
        if(no_8bytes >= 2) {
            size += 16;
            bit_array[bit_array_index++] = 2;
            no_8bytes -= 2;
        }
        ++format;
    }

    if(contains_8) {
        if(no_bytes > 0) {
            ++no_8bytes;
            no_bytes = 0;
        }

    } else if(contains_4) { // no pointers, just size matters

        if(no_bytes == 0) {

        } else if(no_bytes <= 4) {
            size += 4;
        } else {
            size += 8;
        }
    } else {
        size += no_bytes;
    }

    if(no_8bytes == 1) {
        size += 8;
        bit_array[bit_array_index++] = 1;

    } else if(no_8bytes == 2) {
        size += 16;
        bit_array[bit_array_index++] = 2;
    }

    if(!write) {
        return size;
    }

    if(bit_array_index >= HEADER_SIZ * 4 - 1) { // to big string
        return 0;
    }

    /// writes bit array to header, possible to make faster by writing all at same time.
    for(int i = 0; i < bit_array_index; ++i) {
        write_2_bits_first(object , i * 2, bit_array[i]);
    }
    write_2_bits_first(object, 2 * bit_array_index, 0); //Null terminated


    return size;

}



//sets explored to explored
void edit_explored(void *heap, void *object, bool explored) {
    if(hm_get_explored_bit(heap) == explored) {
        if(IS_FORMAT_STRING) {
            SET_FORMAT_STRING;
        } else if(IS_BIT_ARRAY) {
            SET_BIT_ARRAY;
        } else if(IS_SIZE) {
            SET_SIZE;
        }
    } else {
        if(IS_FORMAT_STRING) {
            write_2_bits_last(object, 0, 2);
        } else if(IS_BIT_ARRAY) {
            write_2_bits_last(object, 2, 2);
        } else if(IS_SIZE) {
            write_2_bits_last(object, 2, 3);
        }
    }

}

/// ------------------------SECTION ABOUT BUILDING OM-------------------
///
/// builds a metadata object at where the pointer header is pointing
///
/// \param object place where the object is stored
/// \param size the size of the object to be stored.
void om_build_pointerless(heap_t *heap , void *object, const size_t size) {
    assert(object != NULL);
    assert(object_size_valid(size));//Object is bigger then a chunk - header
    memset(object - HEADER_SIZ, '\0', HEADER_SIZ); //Sets header to 0
    uint64_t *header_data  = HEADER;// header_data is the header
    *header_data = ((uint64_t)size) << 4; //Stores size 4 bits from end
    SET_SIZE;
    edit_explored(heap, object, false);

}



/// Builds a metadata given a src to copy
///
/// \param trg_obj Adress to the object that will get its header built.
/// \param src_obj Adress to object to copy header from.
/// \param format_str_obj Adress to a object that can hold the format string

void om_build_copy(void *trg_obj, const void *src_obj) {
    const void *object = src_obj;
    /*if(IS_FORMAT_STRING){
      strcpy(format_str_obj, get_adress(object));
      om_build_pointerless(format_str_obj,strlen(format_str_obj)+1);
      int last_2 = read_2_bits_last(object, 0);
      *(uint64_t *)(trg_obj-HEADER_SIZ) =(uint64_t)format_str_obj;
      write_2_bits_last(trg_obj, 0, last_2);
      }*/

    const uint64_t *src_header = (HEADER);
    uint64_t *trg_header = (trg_obj - HEADER_SIZ);
    *trg_header = *src_header;

}


/// returns strlen of format string includin '/0'
///
/// \param object Adress to an object
/// \returns length of the format string including '/0' or 0 if no format string exist

size_t om_format_len(const void *object) {
    assert(object);
    if(!IS_FORMAT_STRING) {
        return 0;
    }
    return strlen(get_adress(object)) + 1;
}


/// builds a metadata given a format string.
///
/// \param heap Adress to the heap metadata.
/// \param object Adress to the object, metadata will be stored in the HEADER_SIZ bytes before the object.
/// \param format Format string describing the object according to specification in assignment.

bool om_build(heap_t *heap, void *object, const char *format) {
    assert(object != NULL && format != NULL);
    assert(sizeof(int) == 4 && sizeof(void *) == 8); //needs to be exstended to work for 32 bit
    if(strchr(format, '*') == NULL) {
        om_build_pointerless(heap, object, om_size_format(format));
        return true;
    }
    if(bit_array64(object, format, true) == 0) { // to big, make another object.
        size_t size = strlen(format) + 1;
        char *format_object = hm_get_free_space(heap, size);
        if(!format_object) {
            return false;
        }
        strcpy(format_object, format);
        *(char **)HEADER = format_object;
        om_build_pointerless(heap, format_object, size);
        edit_explored(heap, object, false);
        return true;
    }
    SET_BIT_ARRAY;
    edit_explored(heap, object, false);
    return true;
}



///---------------------SECTION ABOUT SIZE ------------------

/// returns the size of the object
///
/// \param object Adress to an object
/// \returns size of the object

size_t om_size(const void *object) {
    assert(object != NULL);
    if(om_has_forwarding(object)) {
        return om_size(om_get_forwarding(object));
    } else if(IS_FORMAT_STRING) {
        return om_size_format(get_adress(object));
    } else if(IS_BIT_ARRAY) {
        int offset = 0;
        size_t size = 0;
        int data;
        do {
            data = read_2_bits_first(object, offset);
            switch(data) {
            case 1:
            case 3:
                size += 8;
                break;
            case 2:
                size += 16;
                break;
            case 0:
                break;
            default:
                assert(false);// no other values allowed
            }
            offset += 2;
        } while(data != 0 && offset <= 58); //Allows explorer to use bit 61-62 instead of being null terminated
        return size;
    } else if(IS_SIZE) {
        uint64_t *data = (uint64_t *)(HEADER);
        return (uint16_t)(*data >> 4); //reads size 4 bits from end
    }
    return 0;
}

/// returns the size of a format string
///
/// \param format char* specified in the assignment description.
/// \returns size of the format char*

size_t om_size_format(const char *format) {
    return bit_array64(NULL, format, false);
}

//--------------------SECTION ABOUT POINTERS----------------------

/// Returns the amount of pointers stored in the object.
/// Help for building arrays of pointers on the Stack.
///
/// \param object Object with unknown number of pointers inside it.
/// \returns Number of pointers inside the object.

int om_amount_pointers(const void *object) {
    assert(object != NULL);
    if(IS_FORMAT_STRING) {
        const char *format = (char *)get_adress(object);
        int amount = 1;

        do {
            if(*format == '*') {
                ++amount;
            }
            ++format;
        } while(*format);
        return amount;
    } else if(IS_BIT_ARRAY) {

        int offset = 0;
        uint16_t bits_2 = read_2_bits_first(object, offset);
        int amount = 0;
        while(bits_2 != 0) {
            if(bits_2 == 3) {
                ++ amount;
            }
            offset += 2;
            bits_2 = read_2_bits_first(object, offset);
        }
        return amount;

    } else if(om_has_forwarding(object)) {
        return om_amount_pointers(om_get_forwarding(object));
    }

    return 0;
}

/// Gets pointers to the suppoused pointers in the object according to the format string when the object header was builded.
/// Some of them might be NULL or CORRUPTED
///
/// \param object Object to get pointers from.
/// \param pointers Array to store the pointers to pointer in. Get size of array in om_amount_pointers.

void om_get_pointers(const void *object, void ***pointers) {
    assert(object != NULL && pointers != NULL);
    if(IS_FORMAT_STRING) {
        const char *format = (char *)get_adress(object);
        int bytes_before_pointer = 0;
        int index = 1;
        size_t size = om_size_format(format);
        size_t tail_size;
        pointers[0] = (void **)get_adress(object);
        format = strchr(format, '*'); //jumps to first *
        while(format) {
            tail_size = om_size_format(format);
            bytes_before_pointer += size - tail_size;//Head_size = size - tail_size is size of everything from last * to current *

            pointers[index++] = (void **)(object + bytes_before_pointer);

            ++format;// Jumps past the current *
            size = tail_size - HEADER_SIZ;
            bytes_before_pointer += HEADER_SIZ;

            format = strchr(format, '*'); // Jumps to next *
        }
    } else if(IS_BIT_ARRAY) {

        int offset = 0;
        uint16_t bits_2 = read_2_bits_first(object, offset);
        int index = 0;
        int bytes_before_pointer = 0;
        while(bits_2 != 0) {
            switch(bits_2) {
            case 1:
                bytes_before_pointer += HEADER_SIZ;
                break;
            case 2:
                bytes_before_pointer += 2 * HEADER_SIZ;
                break;
            case 3:
                pointers[index++] = (void **)(object + bytes_before_pointer);
                bytes_before_pointer += HEADER_SIZ;
                break;
            default:
                assert(false);//Case that shouldnt exist since 0 is cought in while condition.
                break;
            }
            offset += 2;
            bits_2 = read_2_bits_first(object, offset);//Reads next 2 bits.
        }
    }

    else if(om_has_forwarding(object)) {
        om_get_pointers(om_get_forwarding(object), pointers);
    }

}

/// --------------------SECTION ABOUT FORWARDING--------------------------

/// returns true when object has been moved, otherwise false
///
/// \param object The Object
/// \return If the object has been moved.

bool om_has_forwarding(const void *object) {
    assert(object != NULL);
    return (IS_FORWARDING);
}

/// Void for setting header to forwarding mode (when moving during GC- event)
/// Doesn't move the object, just sets the header to redirect everything to new place.
///
/// \param object Object to set forwarding
/// \param adress New home

void om_set_forwarding(void *object, const void *adress) {
    assert(adress != NULL && object != NULL);
    const void **header = (HEADER);
    *header = adress;
    write_2_bits_last(object, 0, 1);
}

/// gets the forwarding adress if the header is in forwarding mode.
///
/// \param object The object that might have a forwarding adress.
/// \returns The forwarding adress if it has one, else NULL.

void *om_get_forwarding(const void *object) {
    assert(object != NULL);
    if(om_has_forwarding(object)) {
        return get_adress(object);
    } else {
        return NULL;
    }
}

/// Gets is_explored information
///
/// \param object The object.
/// \returns True if the funciton om_toggle_explored has been called a odd number of times.

bool om_get_explored(heap_t *heap, const void *object) {
    return (IS_EXPLORED != hm_get_explored_bit(heap)) || IS_FORWARDING;
}

/// Switches the is_explored between true and false:
/// Default false
///
/// \param object, the object to switch between true and false for om_is_explored.

void om_set_explored(heap_t *heap, void *object) {
    edit_explored(heap, object, true);

}


