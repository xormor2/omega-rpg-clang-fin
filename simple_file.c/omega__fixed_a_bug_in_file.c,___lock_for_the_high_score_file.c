//I fixed a bug in file.c in the omega-rpg source, the C source code.
/* Writes a new high-score file, with the nominated npc as whatever it is */
/* in this particular game, but the others as they appear in the file. */
void save_hiscore_npc(int npc)
{
  FILE *infile, *outfile;
  char buffer[80];
  int i;

  if (gamestatusp(CHEATED))
      return;
  //lock_score_file(); lock that prevented the high score from being saved, I commented it out :-) 8.6.2018: L.K.F.
  strcpy(Str1,Omegavar);
  strcat(Str1,"omega.hi");
  infile = checkfopen(Str1,"rb");
  strcpy(Str2,Omegavar);
#ifdef MSDOS
  strcat(Str2,"omegahi.new");   /* stupid 8.3 msdos filename limit */
#else
  strcat(Str2,"omega.hi.new");
#endif