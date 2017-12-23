

void *gca_alloc_struct(heap_t *heap, char *format)
{
    size_t bytes = om_get_size(format);
    
    void* heap_pointer = hm_get_free_space(heap, bytes);
    
    om_build(heap_pointer, char *format);
    
    return heap_pointer;
    
}

void *gca_alloc_data(heap_t *heap, size_t bytes) {
    
    void* heap_pointer = hm_get_free_space(heap, bytes);
    
    om_build_pointerless(heap_pointer, bytes);
}

/*
size_t get_size(char *format) {
    
    size_t bytes = 0;
    int counter = 0;
    
    while (format != '\0') 
    {
        if (format == '*')
        {
            bytes = bytes + sizeof(void*);
            format++;
            counter+;
            
        } else if (format == 'c')
        {
            bytes = bytes + sizeof(char);
            format++;
            counter+;
            
        } else if (format == 'i')
        {
            bytes = bytes + sizeof(int);
            format++;
            counter+;
            
        } else if (format == 'l')
        {
            bytes = bytes + sizeof(long);
            format++;
            counter+;
          
        } else if (format == 'f')
        {
            bytes = bytes + sizeof(float);
            format++;
            counter+;
            
        } else if (format == 'd')
        {
            bytes = bytes + sizeof(double);
            format++;
            counter+;
        }
        
        format = format - counter;
        return bytes;
    }
}
 */