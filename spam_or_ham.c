#include <stdio.h>
#include <ctype.h>
#include <string.h>

struct Word
{
   char word[100];
   int count;
   double p;
   double q;
};

struct Retset
{
   int num;
   double r;
};

void readTrainFile(char *filename, struct Word *words, int *count);
void readTestFile(char *filename, struct Word *trainWords, int trainCount, struct Retset *retset, char type);
void calcProbability(struct Word *spamWords, struct Word *hamWords, int spamCount, int hamCount);

int main(int argc, char *argv[])
{

   char *trainSpamFile = "datasets/train/dataset_spam_train100.csv";
   char *trainHamFile = "datasets/train/dataset_ham_train100.csv";
   char *testSpamFile = "datasets/test/dataset_spam_test20.csv";
   char *testHamFile = "datasets/test/dataset_ham_test20.csv";
   struct Word spamWords[10000] = {0};

   struct Word hamWords[10000] = {0};
   struct Retset spamRetset[20] = {0};
   struct Retset hamRetset[20] = {0};
   int spamCount = 0;
   int hamCount = 0;

   readTrainFile(trainSpamFile, spamWords, &spamCount);
   readTrainFile(trainHamFile, hamWords, &hamCount);

   calcProbability(spamWords, hamWords, spamCount, hamCount);

   readTestFile(testSpamFile, spamWords, spamCount, spamRetset, 's');
   readTestFile(testHamFile, hamWords, hamCount, hamRetset, 'h');
   return 0;
}

void calcProbability(struct Word *spamWords, struct Word *hamWords, int spamCount, int hamCount)
{

   for (int i = 0; i < spamCount; i++)
   {
      int hc = 1; // If the word has never appeared in the ham, the probability is 1, so the initial value is set to 1.
      for (int j = 0; j < hamCount; j++)
      {
         if (!strcmp(spamWords[i].word, hamWords[j].word))
         {
            hc = hamWords[j].count;
            break;
         }
      }
      spamWords[i].p = (double)spamWords[i].count / (double)(spamWords[i].count + hc);
      spamWords[i].q = 1 - spamWords[i].p;
   }

   for (int i = 0; i < hamCount; i++)
   {
      int sc = 1; // If the word has never appeared in the ham, the probability is 1, so the initial value is set to 1.
      for (int j = 0; j < spamCount; j++)
      {
         if (!strcmp(spamWords[i].word, hamWords[j].word))
         {
            sc = spamWords[j].count;
            break;
         }
      }
      hamWords[i].p = (double)hamWords[i].count / (double)(hamWords[i].count + sc);
      hamWords[i].q = 1 - hamWords[i].p;
   }
}

void readTestFile(char *filename, struct Word *trainWords, int trainCount, struct Retset *retset, char type)
{
   struct Word words[10000] = {0};
   int count = 0;
   int flag = 0;
   int num = 1;
   char tok[13];

   if (type == 's')
   {
      sprintf(tok, "sspamsubject");
   }
   else
   {
      sprintf(tok, "hhamsubject");
   }

   char str_tmp[1024];
   int n = 0;

   FILE *pFile = NULL;

   pFile = fopen(filename, "r");
   if (pFile != NULL)
   {
      fgets(str_tmp, 1024, pFile);
      while (!feof(pFile))
      {
         char temp[1024] = {
             0,
         };
         int x = 0;
         fgets(str_tmp, 1024, pFile);

         for (int i = 0; i < strlen(str_tmp); i++)
         {
            if (isalpha(str_tmp[i]))
            {
               if (isupper(str_tmp[i]))
               {
                  temp[x++] = str_tmp[i] + 32;
               }
               else
               {
                  temp[x++] = str_tmp[i];
               }
            }
            else if (str_tmp[i] == ' ')
            {
               temp[x++] = str_tmp[i];
            }
         }

         char *ptr = strtok(temp, " ");

         while (ptr != NULL)
         {

            if (count != 0 && !strcmp(tok, ptr))
            {
               double pprod = 1.0, qprod = 1.0;
               int noneCheck = 0;
               for (int i = flag; i < count; i++)
               {
                  for (int j = 0; j < trainCount; j++)
                  {
                     if (!strcmp(trainWords[j].word, words[i].word))
                     {
                        pprod *= trainWords[j].p;
                        qprod *= trainWords[j].q;
                        noneCheck = 1;
                        break;
                     }
                  }
               }
               if (noneCheck == 0)
               {
                  retset[num - 1].r = 0.0;
               }
               else
               {
                  retset[num - 1].r = pprod / (pprod + qprod);
               }
               retset[num - 1].num = num;

               num++;

               flag = count;
               n = 0;
               ptr = strtok(NULL, " ");
            }
            else
            {
               for (int i = 0; i <= count; i++)
               {
                  if (!strcmp((words[i]).word, ptr))
                  {
                     (words[i]).count++;
                     n = 1;
                  }
               }
               if (n != 1)
               {
                  strcpy((words[count]).word, ptr);
                  (words[count]).count++;
                  count++;
               }
               n = 0;
               ptr = strtok(NULL, " ");
            }
         }
         if (num == 20)
         {
            double pprod = 1.0, qprod = 1.0;
            int noneCheck = 0;
            for (int i = flag; i < count; i++)
            {
               for (int j = 0; j < trainCount; j++)
               {
                  if (!strcmp(trainWords[j].word, words[i].word))
                  {
                     pprod *= trainWords[j].p;
                     qprod *= trainWords[j].q;
                     noneCheck = 1;
                     break;
                  }
               }
            }
            if (noneCheck == 0)
            {
               retset[num - 1].r = 0.0;
            }
            else
            {
               retset[num - 1].r = pprod / (pprod + qprod);
            }
            retset[num - 1].num = num;
            num++;

            flag = count;
            n = 0;
            ptr = strtok(NULL, " ");
         }
      }
   }
   printf("----------- %s -----------\n", (type == 's') ? "spam" : "ham");
   for (int i = 0; i < num - 1; i++)
   {
      printf("%c - %02d : %.3f\n", type, retset[i].num, retset[i].r);
   }

   fclose(pFile);
}

void readTrainFile(char *filename, struct Word *words, int *count)
{
   char str_tmp[1024];
   int n = 0;

   FILE *pFile = NULL;

   pFile = fopen(filename, "r");
   if (pFile != NULL)
   {
      fgets(str_tmp, 1024, pFile);
      while (!feof(pFile))
      {
         char temp[1024] = {
             0,
         };
         int x = 0;
         fgets(str_tmp, 1024, pFile);

         for (int i = 0; i < strlen(str_tmp); i++)
         {
            if (isalpha(str_tmp[i]))
            {
               if (isupper(str_tmp[i]))
               {
                  temp[x++] = str_tmp[i] + 32;
               }
               else
               {
                  temp[x++] = str_tmp[i];
               }
            }
            else if (str_tmp[i] == ' ')
            {
               temp[x++] = str_tmp[i];
            }
         }

         char *ptr = strtok(temp, " ");
         while (ptr != NULL)
         {
            if (!strcmp("spamsubject", ptr) || !strcmp("hamsubject", ptr))
            {
               n = 0;
               ptr = strtok(NULL, " ");
            }
            else
            {
               for (int i = 0; i <= (*count); i++)
               {
                  if (!strcmp((words[i]).word, ptr))
                  {
                     (words[i]).count++;
                     n = 1;
                  }
               }
               if (n != 1)
               {

                  strcpy((words[(*count)]).word, ptr);
                  (words[(*count)]).count++;
                  (*count)++;
               }
               n = 0;
               ptr = strtok(NULL, " ");
            }
         }
      }
   }
   fclose(pFile);
}