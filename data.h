//donnée qui sera envoyées dans les tubes 
typedef struct _DataPipes {
    int operationToExecute ;
    int key;
    char val[128];
} DataPipes ;

typedef DataPipes *PDataPipes ;

void affichageData(DataPipes data);