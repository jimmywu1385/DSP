#include "hmm.h"
#include<stdio.h>
#include<stdlib.h>
#define MAX_MODEL 20
#define MAX_FILENAME 20
#define MAX_SEQNUM 10000
int main(int argc, char *argv[])
{

    HMM model;
    loadHMM(&model, argv[2]);

    int seq_num = 0;
    int seq_length = 0;
    int sequence[MAX_SEQNUM][MAX_SEQ] = {};   

    FILE *fp = fopen(argv[3], "r");
    char temp[MAX_SEQ] = {};
    while(fscanf(fp, "%s", temp) > 0)
    {
        seq_length = strlen(temp);
        for(int i=0; i<seq_length; ++i)
            sequence[seq_num][i] = temp[i]-65; 
        seq_num += 1;   
    }
    fclose(fp);

    for(int i=0; i<atoi(argv[1]); ++i)
    {
        double pi[6] = {};
        double A[6][6] = {};
        double B[6][6] = {};
        double a_denominator[6] = {};
        double b_denominator[6] = {};

        for(int j=0; j<seq_num; ++j)
        {
            double a[MAX_SEQ][6] = {};
            double b[MAX_SEQ][6] = {};
            double y[MAX_SEQ][6] = {};
            double e[MAX_SEQ-1][6][6] = {};

            for(int k=0; k<6; ++k)
            {
                a[0][k] = model.initial[k]*model.observation[sequence[j][0]][k];
                b[seq_length-1][k] = 1.0;
            }

            for(int k=1; k<seq_length; ++k)
                for(int l=0; l<6; ++l)
                {
                    double sum = 0.0;
                    for(int m=0; m<6; ++m)
                        sum += a[k-1][m]*model.transition[m][l];
                    a[k][l] = sum*model.observation[sequence[j][k]][l];
                }

            for(int k=seq_length-2; k>=0; --k)
                for(int l=0; l<6; ++l)
                    for(int m=0; m<6; ++m)
                        b[k][l] += model.transition[l][m]*model.observation[sequence[j][k+1]][m]*b[k+1][m];
            
            double at_bt = 0.0;
            for(int k=0; k<6; ++k)
                at_bt += a[0][k]*b[0][k];
    
            for(int k=0; k<seq_length; ++k)
                for(int l=0; l<6; ++l)
                    y[k][l] = a[k][l]*b[k][l]/at_bt;
            
            for(int k=0; k<seq_length-1; ++k)
                for(int l=0; l<6; ++l)
                    for(int m=0; m<6; ++m)
                        e[k][l][m] = a[k][l]*model.transition[l][m]*model.observation[sequence[j][k+1]][m]*b[k+1][m]/at_bt;


            for(int k=0; k<seq_length-1; ++k)
                for(int l=0; l<6; ++l)
                    for(int m=0; m<6; ++m)
                        A[l][m] += e[k][l][m];

            for(int k=0; k<6; ++k)
                for(int l=0; l<6; ++l)
                    for(int m=0; m<seq_length; ++m)                    
                        if(sequence[j][m] == k)
                            B[k][l] += y[m][l];


            for(int k=0; k<6; ++k)
                for(int l=0; l<seq_length-1; ++l)
                    a_denominator[k] += y[l][k];

            for(int k=0; k<6; ++k)
                for(int l=0; l<seq_length; ++l)
                    b_denominator[k] += y[l][k];  

            for(int k=0; k<6; ++k)
                pi[k] += y[0][k]/seq_num;    
        }

        for(int j=0; j<6; ++j)
        {
            for(int k=0; k<6; ++k)
            {
                model.transition[j][k] = A[j][k]/a_denominator[j];
                model.observation[j][k] = B[j][k]/b_denominator[k];
            }
        model.initial[j] = pi[j];
        }

    }

    fp = fopen(argv[4], "w");
    dumpHMM(fp, &model);
    fclose(fp);
   
}
