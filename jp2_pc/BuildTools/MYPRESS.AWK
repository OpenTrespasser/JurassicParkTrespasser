BEGIN {
# use gawk -f mypress.awk <fname> <New Extension> to execute this file
   if (ARGC < 3) {
      printf("Not enough parameters\r\n");
   }

   printf("Compressing %s\r\n", ARGV[1])
   sz = ARGV[1];
   i = index(sz, ".");
   szNew = substr(sz, 0, i - 1);

   szCmd = sprintf("lzencode -d %s %s.%s", sz, szNew, ARGV[2]);
   system(szCmd);
}
