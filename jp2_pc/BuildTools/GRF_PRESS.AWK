BEGIN {
# use gawk -f grf_press.awk <fname> <New Extension> to execute this file
   if (ARGC < 3) {
      printf("Not enough parameters\r\n");
   }

#   printf("Compressing %s\r\n", ARGV[1])
   szSrc = ARGV[1];
   i = index(szSrc, ".");

   sz = substr(szSrc, 0, i - 1);

   x = i;

   while (substr(szSrc, x, 1) != "\\") 
   {
      x = x - 1;
#      printf("%i %s %s\r\n", x, substr(szSrc, x, 1), substr(szSrc, x));
   }

   szFName = substr(szSrc, x + 1);
   szName = substr(szSrc, x + 1, (i - 1) - x);
   szExt = substr(szSrc, i);


   szPath = substr(szSrc, 0, x);

   szDst = sprintf("%s%s.%s", szPath, szName, szExt);

#   printf("szPath = %s\r\nszName = %s\r\nszExt = %s\r\n", szPath, szName, szExt);

   szCmd = sprintf("compress -d %s %s%s.%s", szSrc, szPath, szName, ARGV[2]);
   printf("%s\r\n", szCmd);
   if (system(szCmd) == 0)
   {
     szCmd = sprintf("xcopy %s%s.%s %s\\*%s %s", szPath, szName, ARGV[2], ARGV[3], szExt, ARGV[4]);
     printf("%s\r\n", szCmd);
     system(szCmd);
   }

   printf("\r\n");
}

