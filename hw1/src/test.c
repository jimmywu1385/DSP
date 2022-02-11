#include "hmm.h"
#include<string.h>
#define MAX_MODEL 20
#define MAX_FILENAME 20
#define MAX_SEQNUM 10000
int main(int argc, char *argv[])
{
    char model_list[MAX_MODEL][MAX_FILENAME] = {};
    int model_num = 0;

    FILE *fp = fopen(argv[1], "r");
    while(fscanf(fp, "%s", model_list[model_num]) > 0)
        model_num += 1;
    fclose(fp);

    int seq_num = 0;
    int seq_length = 0;
    int sequence[MAX_SEQNUM][MAX_SEQ] = {};   

    fp = fopen(argv[2], "r");
    char temp[MAX_SEQ] = {};
    while(fscanf(fp, "%s", temp) > 0)
    {
        seq_length = strlen(temp);
        for(int i=0; i<seq_length; ++i)
            sequence[seq_num][i] = temp[i]-65; 
        seq_num += 1;   
    }
    fclose(fp);

    double score[MAX_SEQNUM] = {};
    int model_ind[MAX_SEQNUM] = {};
    for(int i=0; i<model_num; ++i)
    {
        HMM model;
        loadHMM(&model, model_list[i]);     
        
        for(int j=0; j<seq_num; ++j)
        {
            double s = 0.0;
            double delta[MAX_SEQ][6] = {};

            for(int k=0; k<6; ++k)
                delta[0][k] = model.initial[k]*model.observation[0][k];

            for(int k=1; k<seq_length; ++k)
                for(int l=0; l<6; ++l)
                {
                    for(int m=0; m<6; ++m)
                        if(delta[k-1][m]*model.transition[m][l]*model.observation[sequence[j][k]][l] > delta[k][l])
                            delta[k][l] = delta[k-1][m]*model.transition[m][l]*model.observation[sequence[j][k]][l];
                }           

            for(int k=0; k<6; ++k)
            {
                if(delta[seq_length-1][k] > s)
                    s = delta[seq_length-1][k];
            }

            if(s > score[j])
            {
                score[j] = s;
                model_ind[j] = i;
            }
        }
    }

    fp = fopen(argv[3], "w");
    for(int i=0; i<seq_num; ++i)
        fprintf(fp, "%s %e\n", model_list[model_ind[i]], score[i]);
    fclose(fp);    
   
}