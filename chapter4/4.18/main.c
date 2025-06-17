#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<linux/types.h>
#include<xfs/xfs.h>
#include<xfs/xfs_format.h>
#include<endian.h>

int
main (int argc, char *argv[])
{
  if (2 != argc)
    {
      fprintf (stderr, "Usage: %s <block device>\n", argv[0]);
      exit (EXIT_FAILURE);
    }
  int fd = open (argv[1], O_RDONLY);
  if (-1 == fd)
    {
      perror ("Error opening device");
      exit (EXIT_FAILURE);
    }

//Seek to supreblock (primary at sector 0, offset 512 bytes)
  if (lseek (fd, 512, SEEK_SET) == -1)
    {
      perror ("Error seeking to superblock");
      close (fd);
      exit (EXIT_FAILURE);
    }

  struct xfs_sb sb;
  if (sizeof (sb) != read (fd, &sb, sizeof (sb)))
    {
      perror ("Error reading superblock");
      exit (EXIT_FAILURE);
    }

  close (fd);

//validate magic number
  if (be32toh (sb.sb_magicnum) != XFS_SB_MAGIC)
    {
      fprintf (stderr, "Not an XFS filesystem!\n");
      exit (EXIT_FAILURE);
    }

  puts ("XFS Superblock Information:\n");
  puts ("============================\n");
  printf ("UUID: %s\n", sb.sb_uuid);


  printf ("Block Size:            %u bytes\n", be32toh (sb.sb_blocksize));
  printf ("Data Blocks:           %llu\n",
	  (unsigned long long) be64toh (sb.sb_dblocks));
  printf ("Inodes:                %llu\n",
	  (unsigned long long) be64toh (sb.sb_icount));
  printf ("Free Inodes:           %llu\n",
	  (unsigned long long) be64toh (sb.sb_ifree));
  printf ("Inode Size:            %u bytes\n", be16toh (sb.sb_inodesize));

  printf ("Log Block Size:        %u bytes\n",
	  be32toh (sb.sb_logsectsize) << sb.sb_blocklog);
  printf ("Journal Device:        ");
  if (be32toh (sb.sb_logstart) == 0)
    printf ("Internal\n");
  else
    printf ("External (0x%llx)\n",
	    (unsigned long long) be64toh (sb.sb_logstart));

  printf ("Features:              ");
  if (be32toh (sb.sb_features2) & XFS_SB_VERSION2_LAZYSBCOUNTBIT)
    printf ("Lazy-SB,");
  if (be32toh (sb.sb_features2) & XFS_SB_VERSION2_ATTR2BIT)
    printf (" Extended-Attrs,");
  printf ("\n");

  return EXIT_SUCCESS;
}
