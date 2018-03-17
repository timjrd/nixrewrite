#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>

#include <iostream>
#include <vector>
#include <set>

using namespace std;

typedef vector<uint8_t> ByteString;

ByteString bs(char const * const str)
{
   ByteString result(strlen(str));
   memcpy(result.data(), str, result.size());
   return result;
}

const int HASH_SIZE = 32;

// omitted: E O U T
//const ByteString BASE32_CHARS = bs("0123456789abcdfghijklmnpqrsvwxyz");

void writeN(const uint8_t c, const int n, FILE * const output)
{
   for (int i = 0; i < n; i++)
      fwrite(&c, 1, 1, output);
}

void rewrite(const ByteString & fromPrefix, const ByteString & toPrefix, const set<ByteString> & hashes, FILE * const input, FILE * const output)
{
   if (toPrefix.size() > fromPrefix.size())
      throw "toPrefix must have at most the same length as fromPrefix";
   
   // bool isBase32[256];
   
   // for (int i = 0; i < 256; i++)
   //    isBase32 = false;
   
   // for (char c : BASE32_CHARS)
   //    isBase32[c] = true;

   ByteString streamPrefix;
   streamPrefix.reserve(fromPrefix.size());

   ByteString streamHash(HASH_SIZE);

   bool end = false;
   
   uint8_t byte = 0;
   
   end = !fread(&byte, 1, 1, input);
   
   while (not end)
   {
      streamPrefix.clear();
      
      while ( not end
              and streamPrefix.size() < fromPrefix.size()
              and byte == fromPrefix[streamPrefix.size()] )
      {
         streamPrefix.push_back(byte);
         end = !fread(&byte, 1, 1, input);
      }

      if (streamPrefix.empty())
      {
         if (not end)
         {
            fwrite(&byte, 1, 1, output);
            end = !fread(&byte, 1, 1, input);
         }
      }
      else if (streamPrefix.size() < fromPrefix.size())
      {
         fwrite(streamPrefix.data(), 1, streamPrefix.size(), output);
      }
      else // if (streamPrefix.size() == fromPrefix.size())
      {
         int slashes = 0;
         while (not end and byte == '/')
         {
            slashes++;
            end = !fread(&byte, 1, 1, input);
         }

         if (slashes == 0 or end)
            fwrite(streamPrefix.data(), 1, streamPrefix.size(), output);
         else
         {
            streamHash[0] = byte;
            int size = fread(streamHash.data()+1, 1, streamHash.size()-1, input);

            end = size < streamHash.size()-1;

            if (not end and hashes.count(streamHash))
            {
               fwrite(toPrefix.data(), 1, toPrefix.size(), output);
               writeN('/', fromPrefix.size() - toPrefix.size(), output);
            }
            else
               fwrite(streamPrefix.data(), 1, streamPrefix.size(), output);

            writeN('/', slashes, output);

            fwrite(streamHash.data(), 1, size+1, output);

            if (not end)
               end = !fread(&byte, 1, 1, input);
         }
      }
   }
}
   
int main(int argc, char ** argv)
{
   if (argc != 4)
   {
      cerr << "usage: " << argv[0] << " <from prefix> <to prefix> <hashes file>" << endl;
      return EXIT_FAILURE;
   }

   const ByteString fromPrefix = bs(argv[1]);
   const ByteString toPrefix   = bs(argv[2]);
   char const * const hashesFileName = argv[3];
   
   set<ByteString> hashes;

   FILE * hashesFile = fopen(hashesFileName, "rb");
   
   ByteString line(HASH_SIZE);
   bool end = false;

   end = fread(line.data(), 1, HASH_SIZE, hashesFile) < HASH_SIZE;
   while (not end)
   {
      hashes.insert(line);

      int c = 0;
      while ( (c = fgetc(hashesFile)) != '\n'
              and not (end = c == EOF) );
      if (not end)
         end = fread(line.data(), 1, HASH_SIZE, hashesFile) < HASH_SIZE;
   }

   fclose(hashesFile);
   
   freopen(NULL, "rb", stdin);
   freopen(NULL, "wb", stdout);
   
   rewrite(fromPrefix, toPrefix, hashes, stdin, stdout);

   fclose(stdin);
   fclose(stdout);
   
   return EXIT_SUCCESS;
}
