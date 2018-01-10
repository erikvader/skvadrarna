///  \file block_writer.h
///  \brief Writes to files in blocks
///
///  Tools for easy saving of datastructures, arrays or any raw memory
///  into files.\n
///  This is done by writing "blocks" of raw bytes into a file.
///  Every "block" can then be individually read from the file to
///  be interpreted as an int or a custom struct or whatever.\n
///  \n
///  example:
///  \code
///  blk_file f = blk_open_save("hej"); //open a file "hej"
///  int i = 5;
///  blk_write(f, sizeof(int), &i);     //write a block containing an int
///  blk_close_file(f);                 //close the file to flush and free memory
///  
///  f = blk_open_read("hej");          //open it again for reading
///  int a;
///  while(blk_read(f, &a)){            //read all blocks in "hej" and interpret them as int
///     printf("%d\n", a);
///  }
///  blk_close_file(f);                 //close
///  \endcode

#ifndef _BLOCK_WRITER_H_
#define _BLOCK_WRITER_H_

#include <stdio.h>
#include <stdbool.h>

/// \typedef blk_file
/// block file object used to store information about the file.
typedef FILE* blk_file;

/// Open a file for block saving
///
/// \p file is the filename as a string
/// The file will be created if it doesn't exist.
/// If it already exists it will be overwritten.\n
/// blk_success_open() is used to check whether the file was opened successfully.
///
/// \param file filename to a file
/// \returns ::blk_file object for the file
/// \see blk_close_file
blk_file blk_open_save(const char *file);

/// Open a file for block reading
///
/// \p file is the filename as a string \n
/// blk_success_open() is used to check whether the file was opened successfully.
///
/// \param file filename to a file
/// \returns ::blk_file object for the file
/// \see blk_close_file
blk_file blk_open_read(const char *file);

/// Checks whether a file was opened successfully
///
/// \param bf a ::blk_file
/// \returns true if \p bf is valid
/// \see blk_open_read blk_open_save
bool blk_success_open(const blk_file bf);

/// Closes a ::blk_file stream
///
/// Frees all memory and stuff used by \p bf.
///
/// \param bf a ::blk_file to close
/// \see blk_open_read blk_open_save
void blk_close_file(blk_file bf);

/// Write raw bytes to a ::blk_file opened using blk_open_save()
///
/// Writes a single block into \p bf.\n
/// The size of the block to be written is \p block_size bytes big.\n
/// \p block_size bytes will be read from \p block, which is a pointer to some data.
///
/// \param bf ::blk_file to write to
/// \param block_size amount of bytes to write to a single block
/// \param block pointer to some data
/// \see blk_open_save
void blk_write(blk_file bf, const size_t block_size, const void *block);

/// Read raw bytes from a ::blk_file opened using blk_open_read()
///
/// Reads a single block from \p bf and stores the result in \p block.\n
/// \p block needs to be atleast blk_next_size() bytes big.
///
/// \param bf ::blk_file to read from
/// \param block pointer to store the read block in
/// \returns true if successfully read something, false otherwise.
/// \see blk_next_size blk_open_read
bool blk_read(blk_file bf, void *block);

/// returns the size in bytes of the next block to be read
///
/// If there aren't anymore blocks to be read this function returns 0.
///
/// \param bf a ::blk_file
/// \returns the size of the next block in bytes, 0 if end of file.
/// \see blk_read
size_t blk_next_size(blk_file bf);

#endif
