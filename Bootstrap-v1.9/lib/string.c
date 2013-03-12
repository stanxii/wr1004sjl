#define size_t unsigned long

void * memset (void * dest,int val, size_t len)
{
  unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

void bcopy (char * src,char *dest,int len)
{
  if (dest < src)
    while (len--)
      *dest++ = *src++;
  else
    {
      char *lasts = src + (len-1);
      char *lastd = dest + (len-1);
      while (len--)
        *(char *)lastd-- = *(char *)lasts--;
    }
}

void * memcpy (void * out, void * in, size_t length)
{
    bcopy(in, out, length);
    return out;
}
