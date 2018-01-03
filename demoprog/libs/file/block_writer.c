#include "block_writer.h"

blk_file blk_open_save(const char *file){
   return fopen(file, "wb");
}

blk_file blk_open_read(const char *file){
   return fopen(file, "rb");
}

bool blk_success_open(const blk_file bf){
   return bf != NULL;
}

void blk_close_file(blk_file bf){
   fclose(bf);
}

void blk_write(blk_file bf, const size_t block_size, const void *block){
   fwrite(&block_size, sizeof(block_size), 1, bf);
   fwrite(block, block_size, 1, bf);
}

bool blk_read(blk_file bf, void *block){
   size_t size = 0;
   size_t read = fread(&size, sizeof(size_t), 1, bf);
   if(read != 1){
      return false;
   }
   fread(block, size, 1, bf);
   return true;
}

size_t blk_next_size(blk_file bf){
   size_t size = 0;
   size_t read = fread(&size, sizeof(size_t), 1, bf);
   if(read != 1){
      size = 0;
   }else{
      fseek(bf, -sizeof(size_t), SEEK_CUR);
   }
   return size;
}
