#include "api_util.h"

int DC_write_all(int fd, const void * buf, size_t count)
{
    ssize_t retcode;
    size_t  offset = 0;
    size_t  bytes_written;
    int     rc = 0;

    if (count == 0) {
        return 0;
    }

    while (1)
    {
        retcode = write(fd, (unsigned char*)buf + offset, count);
        if (retcode < 0)
        {
            rc = errno;
            if (rc == 0) /* handle another thread set errno */
                rc = EIO;
            goto Exit;
        }
        bytes_written = (size_t)retcode;
        if (count == bytes_written) /* write complete */
            break;
        count -= bytes_written;
        offset += bytes_written;
    }
Exit:
    return rc;
}


int DC_read_all(int fd, void *buf, size_t count, int * hitEOF)
{
    ssize_t retcode;
    size_t  offset = 0;
    size_t  bytes_read;
    int     rc = 0;

    if (count == 0) {
        return 0;
    }

    if (hitEOF) {
        *hitEOF = 0;
    }

    while (1) {
        retcode = read(fd, (unsigned char*)buf + offset, count);
        if (retcode < 0) {
            rc = errno;
            if (rc == 0) /* handle another thread set errno */
                rc = EIO;
            goto Exit;
        }
        else if (retcode == 0) {
            if (hitEOF)
                *hitEOF = 1;
            rc = (errno != 0)?errno:-1;
            break;
        }
        bytes_read = (size_t)retcode;
        if (count == bytes_read) /* write complete */
            break;
        count -= bytes_read;
        offset += bytes_read;
    }

Exit:
    return rc;
}

void dcmpi_args_shift(int & argc, char ** argv, int frompos)
{
    int i;
    for (i = frompos; i < (argc-1); i++) {
        argv[i] = argv[i+1];
    }
    argv[argc-1] = NULL;
    argc--;
}
