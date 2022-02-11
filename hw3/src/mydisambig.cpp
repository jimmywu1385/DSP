#include <stdio.h>
#include <string.h>
#include "File.h"
#include "Ngram.h"
#include "VocabMap.h"

#define MAXLINE_LEN 500
#define MAX_STATE 1024
#define MAXLINE 100

Vocab voc, voc_key, voc_value;

double getBigramProb(const char *w1, const char *w2, Ngram &lm)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if(wid1 == Vocab_None)  
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    
    double curP=lm.wordProb( wid2, context);
    return curP;

}

void viterbi(VocabString *word_of_line, char decode[][3], Ngram &lm, VocabMap &map, int len)
{
    int state_len[MAXLINE] = {};
    int backtrack[MAXLINE][MAX_STATE] = {};
    double log_prob[MAXLINE][MAX_STATE] = {};
    int word_ind[MAXLINE][MAX_STATE] = {};

	VocabIndex ind;
    double temp;
	VocabMapIter iter(map, voc_key.getIndex(word_of_line[0]));
	for(int i = 0; iter.next(ind, temp); ++i)
    {
        log_prob[0][i] = getBigramProb("<s>", voc_value.getWord(ind), lm);
		backtrack[0][i] = 0;
        state_len[0] += 1;
        word_ind[0][i] = ind;
	}

    for(int i = 1; i<len; ++i)
    {
        VocabIndex ind;
        double temp;
        VocabMapIter iter(map, voc_key.getIndex(word_of_line[i]));
        for(int j = 0; iter.next(ind, temp); ++j)
        {       
            double max = log_prob[i-1][0]+getBigramProb(voc_value.getWord(word_ind[i-1][0]), voc_value.getWord(ind), lm);          
            VocabIndex max_ind = 0;      
            for(int k = 0; k<state_len[i-1]; ++k)
            {
                double tmp = log_prob[i-1][k];
                tmp += getBigramProb(voc_value.getWord(word_ind[i-1][k]), voc_value.getWord(ind), lm);
                if(tmp > max)
                {
                    max = tmp;
                    max_ind = k;
                }
            }          
            log_prob[i][j] = max;
		    backtrack[i][j] = max_ind;
            state_len[i] += 1;
            word_ind[i][j] = ind;          
        }
    }
    
    int final_max_ind =0;
    double max_prob = log_prob[len-1][0]; 
    for(int i = 1; i<state_len[len-1]; ++i)
    {
        if(log_prob[len-1][i]>max_prob)
        {
            max_prob = log_prob[len-1][i];
            final_max_ind = i;
        }
    }
    
    for(int i = len-1; i>-1; --i)
    {
        strcpy(decode[i], voc_value.getWord(word_ind[i][final_max_ind]));
        final_max_ind = backtrack[i][final_max_ind];
    }
}

int main(int argc, char *argv[])
{
    VocabMap map(voc_key, voc_value);
	Ngram lm(voc, 2);	

	File mapfile(argv[2], "r" );
	map.read(mapfile);
	mapfile.close();

	File lmfile(argv[3], "r" );
	lm.read(lmfile);
	lmfile.close();

    File inputfile(argv[1], "r");
    FILE *fp = fopen(argv[4], "w");   

    char *inputline = NULL;
    while(inputline = inputfile.getline())
    {
		VocabString word_of_line[MAXLINE] = {};
        char decode[MAXLINE][3] = {};
		int len = Vocab::parseWords(inputline, word_of_line, MAXLINE);

        viterbi(word_of_line, decode, lm, map, len);

        char output[MAXLINE_LEN] = {};
        for(int i=0; i<len; ++i)
        {
            strcat(output, decode[i]);
            strcat(output, " ");
        }
        fprintf(fp, "<s> %s</s>\n", output);
    }
    inputfile.close();
    fclose(fp);
}