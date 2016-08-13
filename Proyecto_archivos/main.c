#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct particion{
    char status;
    char type;
    char fit;
    int start;
    int size;
    char name[16];
}particion;

struct particionMontada{
    char path[200];
    char letra;
    int num;
    char id[5];
    char name[16];
}particionMontada;

struct MBR{
    time_t fecha;
    int signature;
    int tam;
    struct particion particiones[4];
}MBR;

struct EBR{
    char status;
    char fit;
    int start;
    int size;
    int next;
    char name[16];
}EBR;

char entrada[500];
char *lista[8];
char *sublista[2];
char *sinComillas;
struct particionMontada Pmontadas[25];
int otro;
int bandera=1;
int KB=1024;
char path[200];
char name[50];
char compara[4];
int f1, f2;
int signature = 0;
int letraab=97;

void empezar(){
    printf("Comando: ");
    leecad(entrada,400);
    printf("ENTRADA: %s\n",entrada);
    SplitEspacio(entrada);
    strcpy(entrada,"");
    empezar();
}

void leecad(char *cad, int n) {
    int i, c;
    i=-1;
    c=getchar();
    if (c == EOF) {
        cad[i] = '\0';
        return;
    }
    else {
        cad[0] = c;
        i = 1;
    }
    for (; i < n-1 && (c=getchar())!=EOF && c!='\n'; i++)
       cad[i] = c;
    cad[i] = '\0';
    if (c != '\n' && c != EOF)
        while ((c = getchar()) != '\n' && c != EOF);
    return;
}

void SplitEspacio(char* entry){
    const char s[2] = " ";
    char *token;
    int x=0;
    while(x<8){
        lista[x]=NULL;
        x=x+1;
    }
    token= malloc(sizeof(char));
    token = strtok(entry, s);
    int contador=0;
    while(token != NULL)
    {
        lista[contador]=malloc(200);
        strcpy(lista[contador],token);
        contador=contador+1;
        token = strtok(NULL, s);
    }

    if(strncmp(lista[0],"#",1)==0){
         printf("Entro a comentario\n");
    }
    else {
        Funcionalidad(lista[0]);
    }
}

void SplitComilla(char* entry){
    const char s[2] = "\"";
    char *token;
    int x=0;
    token= malloc(sizeof(char));
    token = strtok(entry, s);
    int contador=0;
    while(token != NULL)
    {
        sinComillas=malloc(200);
        strcpy(sinComillas,token);
        printf("ESTO HAY EN SIN %s\n",sinComillas);
        contador=contador+1;
        token = strtok(NULL, s);
    }

}

void SplitIgual(char* igual){
    const char s[2] = "::";
    char *token;
    token= malloc(200);
    token = strtok(igual, s);
    int contador=0;
    while( token != NULL )
    {
        sublista[contador]=malloc(200);
        strcpy(sublista[contador],token);
        contador=contador+1;
        token = strtok(NULL, s);
    }
}

void CrearDisco(int tam){
    struct MBR nuevoDisco;
    nuevoDisco.signature=signature;
    nuevoDisco.tam=tam;
    int x = 0;
    while(x<4){
        nuevoDisco.particiones[x].fit='w';
        strcpy(nuevoDisco.particiones[x].name,"");
        nuevoDisco.particiones[x].status='f';
        nuevoDisco.particiones[x].type='t';
        nuevoDisco.particiones[x].size=0;
        nuevoDisco.particiones[x].start=0;
        x=x+1;
    }
    nuevoDisco.fecha= time(NULL);
    printf("sig:%i tam:%i fecha:%ld \n",signature,tam,nuevoDisco.fecha);
    FILE *disco = fopen(path,"w+");
    if(disco!=NULL){
        int aux = tam/1024;
        int x = 0;
        while(x<aux){
            FILE *basura = fopen("/home/wicho/Documentos/archivos/1KB","r");
            if(basura!=NULL){
                char basu[1024];
                int leidos;
                leidos = fread(basu,1,1024,basura);
                if(leidos==1024){
                    fwrite(basu,1,1024,disco);
                }
            }else{
                printf("no hay archivo basura \n");
            }
            fclose(basura);
            x = x +1;
        }
        fclose(disco);
    }else{
        printf("no se pudo crear\n");
    }
    disco = fopen(path,"r+");
    if(disco!=NULL){
        fwrite(&nuevoDisco,sizeof(nuevoDisco),1,disco);
        fclose(disco);
    }else{
        printf("disco creado\n");
    }
    signature = signature +1;
}

void EliminarDisco(char *ruta){
    if(remove(ruta)==0){ // Eliminamos el archivo
       printf("El archivo fue eliminado satisfactoriamente\n");
    }else{
      printf("No se pudo eliminar el archivo\n");
    }
}

void CrearParticion(int tam,char *ruta,char fit,char type,char *name){
    struct MBR discoEditar;
    FILE *disco = fopen(ruta,"r+");
    if(disco!=NULL){
        fread(&discoEditar, sizeof(discoEditar),1,disco);
        int extendidas=0;
        int primarias=0;
        int s = 0;
        int correcto=1;
        while(s<4){
            if(discoEditar.particiones[s].type=='e'){
                extendidas = extendidas +1;
            }
            if(discoEditar.particiones[s].type=='p'){
                primarias = primarias +1;
            }
            s=s+1;
        }
        struct particion nuevaPart;
        nuevaPart.fit = fit;
        strcpy(nuevaPart.name,name);
        nuevaPart.size = tam;
        nuevaPart.status='v';
        nuevaPart.type=type;
        nuevaPart.start=0;
        int aux = 0;
        if(primarias+extendidas==4 && nuevaPart.type!='l'){
            printf("el disco solo puede tener 4 particiones\n");
            correcto=0;
        }else{
            int tam1 = (discoEditar.tam-sizeof(MBR))-nuevaPart.size;
            if(tam1>0){
                if(nuevaPart.type=='l'){
                    correcto =0;
                    int y = 0;
                    int x = 0;
                    while(y<4){
                        if(discoEditar.particiones[y].type=='e'){
                            x=y;
                            y = 4;
                        }
                        y=y+1;
                    }
                    if(y==5){
                        struct EBR nuevalogica;
                        nuevalogica.fit=nuevaPart.fit;
                        strcpy(nuevalogica.name,nuevaPart.name);
                        nuevalogica.next=-1;
                        nuevalogica.size=nuevaPart.size;
                        nuevalogica.status='v';
                        struct EBR aux;
                        fseek(disco,discoEditar.particiones[x].start,SEEK_SET);
                        fread(&aux,sizeof(aux),1,disco);
                        int z= 0;
                        while(z != 1){
                            if(aux.next==-1){
                               int tam2 = discoEditar.particiones[x].size-(aux.start+aux.size)-nuevalogica.size+sizeof(struct EBR);
                               if(tam2>0){
                                   if(aux.status=='f'){
                                       nuevalogica.start=discoEditar.particiones[x].start+sizeof(struct EBR);
                                       fseek(disco,aux.start-sizeof(struct EBR),SEEK_SET);
                                       fwrite(&nuevalogica,sizeof(nuevalogica),1,disco);
                                   }else{
                                       nuevalogica.start=aux.start+aux.size+sizeof(struct EBR);
                                       fseek(disco,aux.start+aux.size,SEEK_SET);
                                       fwrite(&nuevalogica,sizeof(nuevalogica),1,disco);
                                       aux.next=nuevalogica.start-sizeof(struct EBR);
                                       fseek(disco,aux.start-sizeof(struct EBR),SEEK_SET);
                                       fwrite(&aux,sizeof(aux),1,disco);
                                   }
                                }else{
                                   printf("no hay espacio disponible en la particion extendida\n");
                                   correcto=0;
                               }
                               z=1;
                            }else{
                                int tam3 = aux.next-aux.start-aux.size-nuevalogica.size+sizeof(nuevalogica);
                                if(tam3>0){
                                    fseek(disco,aux.start+aux.size,SEEK_SET);
                                    fwrite(&nuevalogica,sizeof(nuevalogica),1,disco);
                                    z=1;
                                }else{
                                    fseek(disco,aux.next,SEEK_SET);
                                    fread(&aux,sizeof(aux),1,disco);
                                }
                            }
                        }
                    }else{
                        printf("no existe una particion extendida\n");
                        correcto=0;
                    }
                }else{
                    while(aux<4){
                        if(discoEditar.particiones[aux].status=='f'){
                            if(aux==0){
                                nuevaPart.start=sizeof(discoEditar);
                            }else{
                                nuevaPart.start=discoEditar.particiones[aux-1].start+discoEditar.particiones[aux-1].size;
                            }
                            if(nuevaPart.size>nuevaPart.start-discoEditar.tam){
                                if(nuevaPart.type=='e'){
                                    if(extendidas==0){
                                        struct EBR logica;
                                        logica.fit=nuevaPart.fit;
                                        strcpy(logica.name,nuevaPart.name);
                                        logica.next=-1;
                                        logica.size=0;
                                        logica.start=nuevaPart.start+sizeof(logica);
                                        logica.status='f';
                                        discoEditar.particiones[aux]=nuevaPart;
                                        fseek(disco,nuevaPart.start,SEEK_SET);
                                        fwrite(&logica,sizeof(logica),1,disco);
                                    }else{
                                        printf("ya existe una extendida\n");
                                        correcto=0;
                                    }
                                }else if(nuevaPart.type=='p'){
                                    if(primarias<3){
                                        discoEditar.particiones[aux]=nuevaPart;
                                    }else{
                                        printf("ya existen 3 primarias\n");
                                        correcto=0;
                                    }
                                }
                                aux=5;
                            }else{
                                printf("ya no queda espacio disponible \n");
                                correcto = 0;
                            }
                        }else{
                            int disponible = 0;
                            if(aux==0){
                                disponible = discoEditar.particiones[aux].start - sizeof(discoEditar);
                            }else{
                                disponible = discoEditar.particiones[aux].start - discoEditar.particiones[aux-1].start + discoEditar.particiones[aux-1].size;
                            }
                            if(disponible>nuevaPart.size){
                                if(aux==0){
                                    nuevaPart.start=sizeof(discoEditar)+sizeof(struct EBR);
                                }else{
                                    nuevaPart.start=discoEditar.particiones[aux-1].start+discoEditar.particiones[aux-1].size;
                                }
                                if(nuevaPart.type=='e'){
                                    if(extendidas==0){
                                        struct EBR logica;
                                        logica.fit=nuevaPart.fit;
                                        strcpy(logica.name,nuevaPart.name);
                                        logica.next=-1;
                                        logica.size=0;
                                        logica.start=nuevaPart.start+sizeof(logica);
                                        logica.status='f';
                                        if(aux==0){
                                            discoEditar.particiones[3]=discoEditar.particiones[2];
                                            discoEditar.particiones[2]=discoEditar.particiones[1];
                                            discoEditar.particiones[1]=discoEditar.particiones[0];
                                            discoEditar.particiones[0]=nuevaPart;
                                        }
                                        if(aux==1){
                                            discoEditar.particiones[3]=discoEditar.particiones[2];
                                            discoEditar.particiones[2]=discoEditar.particiones[1];
                                            discoEditar.particiones[1]=nuevaPart;
                                        }
                                        if(aux==2){
                                            discoEditar.particiones[3]=discoEditar.particiones[2];
                                            discoEditar.particiones[2]=nuevaPart;
                                        }
                                        if(aux==3){
                                            discoEditar.particiones[3]=nuevaPart;
                                        }
                                        fseek(disco,nuevaPart.start,SEEK_SET);
                                        fwrite(&logica,sizeof(logica),1,disco);
                                    }else{
                                        printf("ya existe una extendida \n");
                                        correcto=0;
                                    }
                                }else if(nuevaPart.type=='p'){
                                    if(primarias<3){
                                        if(aux==0){
                                            discoEditar.particiones[3]=discoEditar.particiones[2];
                                            discoEditar.particiones[2]=discoEditar.particiones[1];
                                            discoEditar.particiones[1]=discoEditar.particiones[0];
                                            discoEditar.particiones[0]=nuevaPart;
                                        }
                                        if(aux==1){
                                            discoEditar.particiones[3]=discoEditar.particiones[2];
                                            discoEditar.particiones[2]=discoEditar.particiones[1];
                                            discoEditar.particiones[1]=nuevaPart;
                                        }
                                        if(aux==2){
                                            discoEditar.particiones[3]=discoEditar.particiones[2];
                                            discoEditar.particiones[2]=nuevaPart;
                                        }
                                        if(aux==3){
                                            discoEditar.particiones[3]=nuevaPart;
                                        }
                                    }else{
                                        printf("ya existen 3 primarias \n");
                                        correcto=0;
                                    }
                                }
                                aux=5;
                            }
                        }
                        aux=aux+1;
                    }
                }
                if(aux==4){
                    printf("no hay espacio disponible \n");
                    correcto = 0;
                }
                if(correcto == 1){
                    fseek(disco,0,SEEK_SET);
                    fwrite(&discoEditar,sizeof(discoEditar),1,disco);
                }
            }else{
                printf("la particion es muy grande para el disco\n");
            }
        }
        fclose(disco);
    }else{
        printf("el disco no existe\n");
    }
}

void EliminarParticion(char *ruta,char *name){
    struct MBR discoEditar;
    FILE *disco = fopen(ruta,"r+");
    if(disco!=NULL){
        fread(&discoEditar, sizeof(discoEditar),1,disco);
        int x = 0;
        while(x<4){
            if(strcasecmp(discoEditar.particiones[x].name,name)==0){
                if(x==0){
                    discoEditar.particiones[0]=discoEditar.particiones[1];
                    discoEditar.particiones[1]=discoEditar.particiones[2];
                    discoEditar.particiones[2]=discoEditar.particiones[3];
                    x=4;
                }
                if(x==1){
                    discoEditar.particiones[1]=discoEditar.particiones[2];
                    discoEditar.particiones[2]=discoEditar.particiones[3];
                    x=4;
                }
                if(x==2){
                    discoEditar.particiones[2]=discoEditar.particiones[3];
                    x=4;
                }
                if(x==3){
                    discoEditar.particiones[3].fit='w';
                    strcpy(discoEditar.particiones[3].name,"");
                    discoEditar.particiones[3].status='f';
                    discoEditar.particiones[3].type='t';
                    discoEditar.particiones[3].size=0;
                    discoEditar.particiones[3].start=0;
                    x=4;
                }
            }
            x=x+1;
        }
        if(x==4){
            int y = 0;
            int a = 0;
            while(y<4){
                if(discoEditar.particiones[y].type=='e'){
                    a=y;
                    y = 4;
                }
                y=y+1;
            }
            if(y==5){
                struct EBR aux;
                fseek(disco,discoEditar.particiones[a].start,SEEK_SET);
                fread(&aux,sizeof(aux),1,disco);
                int z= 0;
                struct EBR auxEBR;
                auxEBR=aux;
                while(z != 1){
                    if(aux.next==-1){
                       if(strcasecmp(aux.name,name)==0){
                           if(aux.start==discoEditar.particiones[a].start+sizeof(struct EBR)){
                               struct EBR logica;
                               logica.fit='w';
                               strcpy(logica.name,"");
                               logica.next=-1;
                               logica.size=0;
                               logica.start=discoEditar.particiones[a].start+sizeof(logica);
                               logica.status='f';
                               fseek(disco,discoEditar.particiones[a].start,SEEK_SET);
                               fwrite(&logica,sizeof(logica),1,disco);
                           }else{
                               char basu[sizeof(struct EBR)];
                               int h = 0;
                               while(h<sizeof(struct EBR)){
                                   basu[h]='/';
                                   basu[h+1]='0';
                                   h = h + 2;
                               }
                               int nuevonext = aux.next;
                               fseek(disco,aux.start,SEEK_SET);
                               fwrite(&basu,sizeof(basu),1,disco);
                               auxEBR.next=nuevonext;
                               fseek(disco,auxEBR.start,SEEK_SET);
                               fwrite(&auxEBR,sizeof(auxEBR),1,disco);
                           }
                       }else{
                           printf("la particion no existe \n");
                       }
                       z=1;
                    }else{
                        if(strcasecmp(aux.name,name)==0){
                            if(aux.start!=discoEditar.particiones[a].start+sizeof(struct EBR)){
                                char basu[sizeof(struct EBR)];
                                int h = 0;
                                while(h<sizeof(struct EBR)){
                                    basu[h]='/';
                                    basu[h+1]='0';
                                    h = h + 2;
                                }
                                int nuevonext = aux.next;
                                fseek(disco,aux.start-sizeof(struct EBR),SEEK_SET);
                                fwrite(&basu,sizeof(basu),1,disco);
                                auxEBR.next=nuevonext;
                                fseek(disco,auxEBR.start-sizeof(struct EBR),SEEK_SET);
                                fwrite(&auxEBR,sizeof(auxEBR),1,disco);
                            }else{
                                fseek(disco,aux.start-sizeof(struct EBR),SEEK_SET);
                                aux.status='f';
                                fwrite(&aux,sizeof(aux),1,disco);
                            }
                            z=1;
                        }else{
                            fseek(disco,aux.next,SEEK_SET);
                            auxEBR=aux;
                            fread(&aux,sizeof(aux),1,disco);
                        }
                    }
                }
            }
        }else{
            fseek(disco,0,SEEK_SET);
            fwrite(&discoEditar,sizeof(discoEditar),1,disco);
            printf("particion eliminada \n");
        }
        fclose(disco);
    }else{
        printf("disco no encontrado \n");
    }
}

void Montar(char *ruta,char *name){
    struct MBR discoEditar;
    FILE *disco = fopen(ruta,"r+");
    int encontrado = 0;
    printf("Ruta: %s \n",ruta);
    if(disco!=NULL){
        fread(&discoEditar, sizeof(discoEditar),1,disco);
        int x = 0;
        while(x<4){
            if(strcasecmp(discoEditar.particiones[x].name,name)==0){
                encontrado = 1;
                x=4;
            }
            x=x+1;
        }
        if(x==4){
            int y = 0;
            int a = 0;
            while(y<4){
                if(discoEditar.particiones[y].type=='e'){
                    a=y;
                    y = 4;
                }
                y=y+1;
            }
            if(y==5){
                struct EBR aux;
                fseek(disco,discoEditar.particiones[a].start,SEEK_SET);
                fread(&aux,sizeof(aux),1,disco);
                int z= 0;
                while(z != 1){
                    if(aux.next==-1){
                       if(strcasecmp(aux.name,name)==0){
                           encontrado=1;
                       }else{
                           printf("la particion no existe \n");
                       }
                       z=1;
                    }else{
                        if(strcasecmp(aux.name,name)==0){
                            encontrado=1;
                            z=1;
                        }else{
                            fseek(disco,aux.next,SEEK_SET);
                            fread(&aux,sizeof(aux),1,disco);
                        }
                    }
                }
            }
        }else{
            printf("particion encontrada \n");
        }
    }else{
        printf("disco no encontrado \n");
    }
    fclose(disco);
    if(encontrado==1){
        struct particionMontada nuevo;
        strcpy(nuevo.name,name);
        strcpy(nuevo.path,ruta);
        nuevo.num=1;
        int con=0;
        while(Pmontadas[con].num!=0){
            if(strcasecmp(Pmontadas[con].path, ruta)==0){
                nuevo.num=Pmontadas[con].num+1;
                nuevo.letra=Pmontadas[con].letra;
            }
            con = con + 1;
        }
        if(nuevo.num==1){
            nuevo.letra=letraab + 0;
            letraab = letraab +1;
        }
        nuevo.id[0]='v';
        nuevo.id[1]='d';
        nuevo.id[2]=nuevo.letra;
        nuevo.id[3]= nuevo.num + '0';
        Pmontadas[con]=nuevo;
        printf("id: %s \n",nuevo.path);
    }
}

void Desmontar(char *id){
    int con = 0;
    int aux = 0;
    while(Pmontadas[con].num!=0){
        if(aux==0){
            if(strcasecmp(Pmontadas[con].id,id)==0){
                aux=1;
                Pmontadas[con]=Pmontadas[con+1];
            }
        }else{
            Pmontadas[con]=Pmontadas[con+1];
        }
        con=con+1;
    }
    if(aux==0){
        printf("no se encontro la particion \n");
    }else{
       printf("la particion fue desmontada \n");
    }
}

void Reportes(char *name,char *ruta, char *id){
    FILE *reporte = fopen(ruta,"w+");
    if(reporte!=NULL){
        int con = 0;
        int aux = 0;
        while(Pmontadas[con].num!=0){
            if(aux==0){
                if(strcasecmp(Pmontadas[con].id,id)==0){
                    aux=1;
                }
            }
            if(aux==1){
                break;
            }
            con=con+1;
        }
        if(aux==0){
            printf("la particion no existe \n");
        }else{
            if(strcasecmp(name,"mbr")==0){
               fprintf(reporte,"digraph G {\n");
               fprintf(reporte,"   node [shape=plaintext]\n");
               fprintf(reporte,"   a [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
               FILE *disco = fopen(Pmontadas[con].path,"r+");
               if(disco!=NULL){
                   if(strcasecmp(name,"mbr")==0){
                       struct MBR discoEditar;
                       fread(&discoEditar, sizeof(discoEditar),1,disco);
                       fprintf(reporte,"       <tr><td colspan=\"2\"><b>Reporte MBR</b></td></tr>\n");
                       fprintf(reporte,"       <tr><td>MBR</td><td> %s </td></tr>\n",Pmontadas[con].path);
                       fprintf(reporte,"       <tr><td>MBR tamaño</td><td> %i </td></tr>\n",discoEditar.tam);
                       struct tm *tlocal = localtime(&discoEditar.fecha);
                       char fecha[60];
                       strftime(fecha,60,"%d/%m/%y %H:%M:%S",tlocal);
                       fprintf(reporte,"       <tr><td>MBR fecha</td><td> %s </td></tr>\n",fecha);
                       fprintf(reporte,"       <tr><td>MBR signature</td><td> %i </td></tr>\n",discoEditar.signature);
                       int x = 0;
                       while(discoEditar.particiones[x].status=='v'&& x<4){
                           fprintf(reporte,"       <tr><td>part status %i</td><td> %c </td></tr>\n",x,discoEditar.particiones[x].status);
                           fprintf(reporte,"       <tr><td>part type %i</td><td> %c </td></tr>\n",x,discoEditar.particiones[x].type);
                           fprintf(reporte,"       <tr><td>part fit %i</td><td> %c </td></tr>\n",x,discoEditar.particiones[x].fit);
                           fprintf(reporte,"       <tr><td>part start %i</td><td> %i </td></tr>\n",x,discoEditar.particiones[x].start);
                           fprintf(reporte,"       <tr><td>part size %i</td><td> %i </td></tr>\n",x,discoEditar.particiones[x].size);
                           fprintf(reporte,"       <tr><td>part name %i</td><td> %s </td></tr>\n",x,discoEditar.particiones[x].name);
                           x=x+1;
                       }
                       int y = 0;
                       int a = 0;
                       while(y<4){
                           if(discoEditar.particiones[y].type=='e'){
                               a=y;
                               y = 4;

                           }
                           y=y+1;
                       }
                       if(y==5){
                           struct EBR aux;
                           fseek(disco,discoEditar.particiones[a].start,SEEK_SET);
                           fread(&aux,sizeof(aux),1,disco);
                           int z= 0;
                           while(z != 1){
                               if(aux.next==-1){
                                  fprintf(reporte,"         <tr><td colspan=\"2\"><b>Reporte EBR</b></td></tr>\n");
                                  fprintf(reporte,"         <tr><td>part status </td><td> %c </td></tr>\n",aux.status);
                                  fprintf(reporte,"         <tr><td>part fit </td><td> %c </td></tr>\n",aux.fit);
                                  fprintf(reporte,"         <tr><td>part start </td><td> %i </td></tr>\n",aux.start);
                                  fprintf(reporte,"         <tr><td>part size </td><td> %i </td></tr>\n",aux.size);
                                  fprintf(reporte,"         <tr><td>part next </td><td> %i </td></tr>\n",aux.next);
                                  fprintf(reporte,"         <tr><td>part name </td><td> %s </td></tr>\n",aux.name);
                                  z=1;
                               }else{
                                   fprintf(reporte,"         <tr><td colspan=\"2\"><b>Reporte EBR</b></td></tr>\n");
                                   fprintf(reporte,"         <tr><td>part status </td><td> %c </td></tr>\n",aux.status);
                                   fprintf(reporte,"         <tr><td>part fit </td><td> %c </td></tr>\n",aux.fit);
                                   fprintf(reporte,"         <tr><td>part start </td><td> %i </td></tr>\n",aux.start);
                                   fprintf(reporte,"         <tr><td>part size </td><td> %i </td></tr>\n",aux.size);
                                   fprintf(reporte,"         <tr><td>part next </td><td> %i </td></tr>\n",aux.next);
                                   fprintf(reporte,"         <tr><td>part name </td><td> %s </td></tr>\n",aux.name);
                                   fseek(disco,aux.next,SEEK_SET);
                                   fread(&aux,sizeof(aux),1,disco);
                               }
                           }
                       }
                   }
               }
               fprintf(reporte,"   </table>>];\n");
               fprintf(reporte," }\n");
               fclose(disco);
            }
        }
         if(strcasecmp(name,"disk")==0){
               FILE *disco = fopen(Pmontadas[con].path,"r+");
               fprintf(reporte,"digraph G {\n");
               fprintf(reporte,"   node [shape=plaintext]\n");
               fprintf(reporte,"   a [label=<<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n");
               if(disco!=NULL){
                   if(strcasecmp(name,"disk")==0){
                       struct MBR discoEditar;
                       fread(&discoEditar, sizeof(discoEditar),1,disco);
                       fprintf(reporte,"       <tr><td colspan=\"3\"><b>MBR</b></td></tr>\n");
                       if(discoEditar.particiones[0].start==sizeof(MBR)){
                            if(discoEditar.particiones[0].type!='e'){
                                fprintf(reporte,"       <tr><td colspan=\"3\"><b>primaria: %s</b></td></tr>\n",discoEditar.particiones[0].name);
                            }else{
                                fprintf(reporte,"       <tr><td rowspan=\"10\"><b>Extendida: %s</b></td></tr>\n",discoEditar.particiones[0].name);
                                struct EBR aux;
                                fseek(disco,discoEditar.particiones[0].start,SEEK_SET);
                                fread(&aux,sizeof(aux),1,disco);
                                int z= 0;
                                while(z != 1){
                                    if(aux.next==-1){
                                       fprintf(reporte,"         <tr><td colspan=\"2\"><b>libre</b></td></tr>\n");
                                       z=1;
                                    }else{
                                        fprintf(reporte,"         <tr><td colspan=\"2\"><b>EBR</b></td></tr>\n");
                                        if(aux.size!=0){
                                            if(aux.start==discoEditar.particiones[0].start+sizeof(EBR)){
                                                fprintf(reporte,"         <tr><td colspan=\"2\"><b>logica</b></td></tr>\n");
                                            }else{
                                                fprintf(reporte,"         <tr><td colspan=\"2\"><b>libre</b></td></tr>\n");
                                                fprintf(reporte,"         <tr><td colspan=\"2\"><b>logica</b></td></tr>\n");
                                            }
                                            fprintf(reporte,"         <tr><td colspan=\"2\"><b>libre</b></td></tr>\n");
                                        }

                                        fseek(disco,aux.next,SEEK_SET);
                                        fread(&aux,sizeof(aux),1,disco);
                                    }
                                }
                            }
                       }else{
                            fprintf(reporte,"       <tr><td colspan=\"3\"><b>libre</b></td></tr>\n");
                            fprintf(reporte,"       <tr><td colspan=\"3\"><b>primaria: %s</b></td></tr>\n",discoEditar.particiones[0].name);
                       }
                       int x = 1;
                       while(discoEditar.particiones[x].status=='v'&& x<4){
                           if(discoEditar.particiones[x].start==discoEditar.particiones[x-1].start+discoEditar.particiones[x-1].size){
                                if(discoEditar.particiones[x].type!='e'){
                                    fprintf(reporte,"       <tr><td colspan=\"3\"><b>primaria: %s</b></td></tr>\n",discoEditar.particiones[x].name);
                                }else{
                                    fprintf(reporte,"       <tr><td rowspan=\"10\"><b>Extendida: %s</b></td></tr>\n",discoEditar.particiones[x].name);
                                    struct EBR aux;
                                    fseek(disco,discoEditar.particiones[x].start,SEEK_SET);
                                    fread(&aux,sizeof(aux),1,disco);
                                    int z= 0;
                                    while(z != 1){
                                        if(aux.next==-1){
                                           fprintf(reporte,"         <tr><td colspan=\"2\"><b>libre</b></td></tr>\n");
                                           z=1;
                                        }else{
                                            fprintf(reporte,"         <tr><td colspan=\"2\"><b>EBR</b></td></tr>\n");
                                            if(aux.size!=0){
                                                if(aux.start==discoEditar.particiones[x].start+sizeof(EBR)){
                                                    fprintf(reporte,"         <tr><td colspan=\"2\"><b>logica</b></td></tr>\n");
                                                }else{
                                                    fprintf(reporte,"         <tr><td rowspan=\"2\"><b>libre</b></td></tr>\n");
                                                    fprintf(reporte,"         <tr><td colspan=\"2\"><b>logica</b></td></tr>\n");
                                                }
                                                fprintf(reporte,"         <tr><td colspan=\"2\"><b>libre</b></td></tr>\n");
                                            }

                                            fseek(disco,aux.next,SEEK_SET);
                                            fread(&aux,sizeof(aux),1,disco);
                                        }
                                    }
                                }
                           }else{
                                fprintf(reporte,"       <tr><td colspan=\"3\"><b>libre</b></td></tr>\n");
                                fprintf(reporte,"       <tr><td colspan=\"3\"><b>primaria: %s</b></td></tr>\n",discoEditar.particiones[x].name);
                           }
                           x=x+1;
                       }
               }
               fprintf(reporte,"   </table>>];\n");
               fprintf(reporte," }\n");
               fclose(disco);
            }
        }
        fclose(reporte);
    }
    char comand[200];
    strcpy(comand,"");
    strcat(comand,"dot ");
    strcat(comand,ruta);
    strcat(comand," -o ");
    strcat(comand,ruta);
    strcat(comand,".png -Tpng -Gcharset=utf8");
    system(comand);
}

void ListarMont(){
    int cont = 0;
    while(Pmontadas[cont].letra!='k'){
        printf("id: %s name: %s path: %s \n",Pmontadas[cont].id,Pmontadas[cont].name,Pmontadas[cont].path);
        cont = cont +1;
    }
}

void Funcionalidad(char* token){

    bandera = 1;
//IF DEL COMANDO MKDISK---------------------------------------------------------------------------------------
        if (strcasecmp(token, "mkdisk")==0){
            printf("Entro mkdisk\n");
            int size=0;
            char unit[1];
            int tam=KB*KB;
            int sizec=0;
            int pathc=0;
            int namec=0;
            int cont = 1;
            strcpy(path,"");
            strcpy(name,"");
            while (lista[cont]!=NULL){
                SplitIgual(lista[cont]);

                if(strcasecmp(sublista[0],"-size")==0){
                    if(atoi(sublista[1])>=0){
                        size=atoi(sublista[1]);
                        printf("ESTO HAY EN TAMAÑO %d\n",size);
                        otro=size;
                    }
                    else{
                        printf("El tamaño que ingreso es invalido\n");
                        bandera=0;
                    }
                    sizec=1;
                }

                if(strcasecmp(sublista[0],"+unit")==0){
                    strcpy(unit,sublista[1]);
                    printf("ESTO HAY EN UNIT: %s\n",unit);
                    printf("ESTO HAY EN SUBLISTA: %s\n",sublista[0]);
                    if(strcasecmp(unit,"K")==0){
                        printf("VIENE K");
                        tam=KB;
                        printf("Esto tiene otro %d\n",size);
                        printf("Esto tiene K %d\n",tam);

                    }
                    else if(strcasecmp(unit,"M")==0){
                        printf("VIENE M");
                        printf("Esto tiene size %d\n",size);
                        printf("Esto tiene M %d\n",tam);
                    }else{
                        printf("unidad erronea \n");
                    }
                }

                if(strcasecmp(sublista[0],"-path")==0){
                    strcpy(path,sublista[1]);
                    printf("ESTO HAY EN PATH: %s\n",path);
                    SplitComilla(path);
                    strcpy(path,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",sinComillas);
                    strcat(path,name);
                    pathc=1;
                }

                if(strcasecmp(sublista[0],"-name")==0){
                    strcpy(name,sublista[1]);
                    printf("ESTO HAY EN NAME: %s\n",name);
                    SplitComilla(name);
                    strcpy(name,sinComillas);
                    printf("ESTO HAY EN NAME: %s\n",name);
                    strcat(path,name);
                    char name1[50];
                    strcpy(name1,name);
                    char *dsk[2];
                    const char s[2] = ".";
                    char *token;
                    token= malloc(200);
                    token = strtok(name1, s);
                    int contador=0;
                    dsk[0]=malloc(200);
                    dsk[1]=malloc(200);
                    while( token != NULL )
                    {
                        strcpy(dsk[contador],token);
                        printf("ESTO HAY EN NAME: %s\n",dsk[contador]);
                        contador=contador+1;
                        token = strtok(NULL, s);
                    }
                    if(strcasecmp(dsk[1],"dsk")==0){
                        namec=1;
                    }else{
                        printf("Falta extension .dsk \n");
                    }
                }

                cont = cont+1;
            }
            tam=tam*otro;
            printf("Esto tiene tam %d\n",tam);
            printf("ESTO HAY EN PATH: %s\n",path);

            if(sizec==1 && pathc==1 && namec == 1){
                CrearDisco(tam);
            }else{
                printf("Falta uno o mas parametros obligatorios \n");
            }

        }

//IF DEL COMANDO RMDISK---------------------------------------------------------------------------------------
        else if (strcasecmp(token,"rmdisk")==0){
            printf("Entro rmdisk");
            char *ruta;
            int pathc=0;
            ruta=malloc(sizeof(200));
            int cont =1;
            while(lista[cont]!=NULL){
                SplitIgual(lista[cont]);

                if(strcasecmp(sublista[0],"-path")==0){
                    strcpy(ruta,sublista[1]);
                    printf("ESTO HAY EN PATH: %s\n",ruta);
                    SplitComilla(ruta);
                    strcpy(ruta,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",sinComillas);
                    pathc=1;
                }

                cont = cont+1;
            }

            if(pathc==1){
                printf("Desea eliminar el archivo: %s(S/N)\n",ruta);
                leecad(compara,2);
                if(strcasecmp(compara, "N") == 0){
                   printf("El archivo no fue eliminado\n");
                }
                else if(strcasecmp(compara, "S") == 0){
                    EliminarDisco(ruta);
                }
                else{
                    printf("La opcion que ingreso es incorrecta\n");

                }
            }else{
                printf("Falta uno o mas parametros obligatorios \n");
            }
       }

//IF DEL COMANDO FDISK---------------------------------------------------------------------------------------

        else if (strcasecmp(token, "fdisk")==0){
            printf("Entro fdisk");
            bandera=1;
            f1=0;
            f2=0;
            int size=0;
            char fit='w';
            char *del;
            int tam=KB;
            char *ruta=NULL;
            char *name=NULL;
            char type='p';
            int cont = 1;
            while(lista[cont]!=NULL){
                SplitIgual(lista[cont]);
                if(strcasecmp(sublista[0],"-size")==0){
                    printf("Viene -size");
                    if(atoi(sublista[1])>=0){
                        size=atoi(sublista[1]);
                        printf("ESTO HAY EN TAMAÑO %d\n",size);
                        otro=size;
                    }
                }
                if(strcasecmp(sublista[0],"+unit")==0){
                    printf("ESTO HAY EN SUBLISTA: %s\n",sublista[0]);
                    if(strcasecmp(sublista[1],"K")==0){
                        printf("VIENE K");
                        tam=KB;
                        printf("Esto tiene otro %d\n",otro);
                        printf("Esto tiene K %d\n",tam);

                    }
                    else if(strcasecmp(sublista[1],"M")==0){
                        printf("VIENE M");
                        tam=KB*KB;
                        printf("Esto tiene size %d\n",otro);
                        printf("Esto tiene M %d\n",tam);
                    }
                    else if(strcasecmp(sublista[1],"B")==0){
                        printf("VIENE B");
                        tam=1;
                        printf("Esto tiene size %d\n",otro);
                        printf("Esto tiene B %d\n",tam);
                    }
                    else{
                        printf("DEBE DE ASIGNARLE UN PARAMETRO PERMITIDO A -UNIT \n");
                        bandera=0;
                    }
                }

                if(strcasecmp(sublista[0],"-path")==0){
                    ruta=malloc(200);
                    strcpy(ruta,sublista[1]);
                    printf("ESTO HAY EN PATH: %s\n",ruta);
                    SplitComilla(ruta);
                    strcpy(ruta,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",ruta);
                }

                if (strcasecmp(sublista[0],"+type")==0){
                    printf("Entro -type");
                    if(strcasecmp(sublista[1],"P")==0){
                        type = 'p';
                    }
                    else if(strcasecmp(sublista[1],"E")==0){
                        type = 'e';

                    }
                    else if(strcasecmp(sublista[1],"L")==0){
                        type ='l';
                    }
                    else{
                        printf("Debe asignarle un valor permitido a -type\n");
                        bandera=0;
                    }

                }

                if(strcasecmp(sublista[0],"+fit")==0){
                    printf("Entro -fit");
                    if (strcasecmp(sublista[1],"BF")==0){
                        fit = 'b';
                    }
                    else if (strcasecmp(sublista[1],"FF")==0){
                        fit = 'f';
                    }
                    else if (strcasecmp(sublista[1],"WF")==0){
                        fit = 'w';
                    }
                    else{
                        printf("Asignele un valor correcto a -fit\n");
                    }

                }

                if(strcasecmp(sublista[0],"-name")==0){
                    printf("Entro -name");
                    name=malloc(50);
                    strcpy(name,sublista[1]);
                    SplitComilla(name);
                    strcpy(name,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",name);
                }

                if(strcasecmp(sublista[0],"+delete")==0){
                    printf("VIENE -delete\n");
                    f1=1;
                    if(strcasecmp(sublista[1],"fast")==0){
                        del=malloc(4);
                        strcpy(del,sublista[1]);
                    }
                    else if(strcasecmp(sublista[1],"full")==0){
                        del=malloc(4);
                        strcpy(del,sublista[1]);
                    }
                    else{
                        printf("Debe asignar un valor correcto a -delete\n");
                        bandera=0;
                    }
                }

                if(strcasecmp(sublista[0],"+add")==0){
                    printf("VIENE -add\n");
                    f2=1;
                    if(atoi(sublista[1])>=0){
                        size=atoi(sublista[1]);
                        printf("ESTO HAY EN SIZE %d\n",size);
                        otro = size;
                    }
                    else{
                        printf("Debe asignarle un valor correcto a -add\n");
                        bandera=0;
                    }
                }
                cont = cont +1;
            }
            printf("bandera %i",bandera);
            tam = otro * tam;
            printf("ESTO TRAEN LAS F'S: %d, %d \n", f1,f2);
            if(ruta==NULL){
                printf("no viene una path \n");
                bandera=0;
            }
            if(name==NULL){
                printf("no viene nombre de la particion\n");
                bandera=0;
            }

            if (bandera==1){
                if(f1==0 && f2==0){
                    if(size!=0){
                        CrearParticion(tam,ruta,fit,type,name);
                    }else{
                        printf("no viene size\n");
                    }
                }
                if(f1==1){
                    EliminarParticion(ruta,name);
                }
                if(f2==1){
                    //AgregarEspacio(ruta,tam,add,name);

                }
            }

         }

//IF DEL COMANDO MOUNT---------------------------------------------------------------------------------------
        else if (strcasecmp(token, "mount")==0){
            printf("Entro mount \n");
            char *nombre;
            char *ruta;
            bandera = 2;
            if(lista[1]!=NULL){
                SplitIgual(lista[1]);
                if(strcasecmp(sublista[0],"-path")==0){
                    ruta=malloc(200);
                    strcpy(ruta,sublista[1]);
                    SplitComilla(ruta);
                    strcpy(ruta,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",ruta);
                    bandera = 1;
                }
                else{
                    printf("SE ESPERABA -PATH \n");
                    bandera=0;
                }
            }
            if(lista[2]!=NULL){
                SplitIgual(lista[2]);
                if(strcasecmp(sublista[0],"-name")==0){
                    nombre=malloc(50);
                    strcpy(nombre,sublista[1]);
                    SplitComilla(nombre);
                    strcpy(nombre,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",nombre);
                }
                else{
                    printf("SE ESPERABA -NAME \n");
                    bandera=0;
                }
            }
            if(bandera==1){
                Montar(ruta,nombre);
            }
            if(bandera == 2){
                ListarMont();
            }
            if(bandera == 0){
                printf("Faltan Parametros \n");
            }
        }

//IF DEL COMANDO UNMOUNT---------------------------------------------------------------------------------------
        else  if (strcasecmp(token, "unmount")==0){
            printf("Entro unmount \n");
            char *id;
            if(lista[1]!=NULL){
                SplitIgual(lista[1]);
                if(strcasecmp(sublista[0],"-id")==0){
                    id=malloc(4);
                    strcpy(id,sublista[1]);
                }
                else{
                    printf("Se esperaba el comando -id \n");
                    bandera=0;
                }
            }
            if(bandera==1){
                Desmontar(id);
            }

        }


//IF DEL COMANDO REP---------------------------------------------------------------------------------------
        else if (strcasecmp(token, "rep")==0){
            printf("Entro rep");
            char *name;
            char *ruta;
            char *id;
            bandera=1;
            int cont = 1;
            while(lista[cont]!=NULL){
                SplitIgual(lista[cont]);
                if(strcasecmp(sublista[0],"-name")==0){
                    name=malloc(10);
                    if(strcasecmp(sublista[1],"\"mbr\"")==0 || strcasecmp(sublista[1],"\"disk\"")==0){
                        strcpy(name,sublista[1]);
                        SplitComilla(name);
                        strcpy(name,sinComillas);
                        printf("ESTO HAY EN NAME: %s\n",name);
                    }
                    else{
                        bandera=0;
                        printf("Ingrese un nombre correcto para el reporte\n");
                    }
                }

                if(strcasecmp(sublista[0],"-path")==0){
                    ruta=malloc(200);
                    strcpy(ruta,sublista[1]);
                    SplitComilla(ruta);
                    strcpy(ruta,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",ruta);
                }

                if(strcasecmp(sublista[0],"-id")==0){
                    id=malloc(10);
                    strcpy(id,sublista[1]);

                }
                cont = cont +1;
            }

            if(bandera==1){
                printf("ENTRO A REPORTES\n");
                Reportes(name,ruta,id);
            }
        }

//IF DEL COMANDO EXEC---------------------------------------------------------------------------------------
        else if (strcasecmp(token, "exec")==0){
            printf("Entro exec \n");
            bandera=1;
            char *ruta;
            if(lista[1]!=NULL){
                SplitIgual(lista[1]);
                if(strcasecmp(sublista[0],"-path")==0){
                    ruta=malloc(200);
                    strcpy(ruta,sublista[1]);
                    SplitComilla(ruta);
                    strcpy(ruta,sinComillas);
                    printf("ESTO HAY EN PATH: %s\n",ruta);
                }
                else{
                    printf("SE ESPERABA -PATH");
                    bandera=0;
                }
                if(bandera==1){
                    //CODIGO QUE SIRVE PARA LEER EL ARCHIVO LINEA POR LINEA
                    char linea[300];
                    FILE *archEntrada=fopen(ruta,"r");
                    if(archEntrada){
                        while(fgets(linea,300,archEntrada)!=NULL){
                            int i;
                            char Aux[2];
                            Aux[1]='\0';
                            char lineaAux[300];
                            strcpy(lineaAux,"");
                            for(i=0; i<strlen(linea);i++){
                                Aux[0]=linea[i];
                                if(Aux[0]=='\n' || Aux[0]=='\r'){
                                    if(i==0){
                                        strcpy(lineaAux,"\n");
                                        break;
                                    }
                                }
                                else{
                                    strcat(lineaAux,Aux);
                                }

                            }
                            printf("comando %s\n",lineaAux);
                            SplitEspacio(lineaAux);
                        }
                        fclose(archEntrada);
                    }
                    else{
                        printf("LA RUTA ES INCORRECTA \n");
                    }

                }
            }
       }

//IF DEL COMANDO EXIT---------------------------------------------------------------------------------------
       else if(strcasecmp(token,"exit")==0){
            exit(0);
       }

}


int main()
{
    int x = 0;
    while(x<25){
        strcpy(Pmontadas[x].id,"");
        Pmontadas[x].letra='k';
        strcpy(Pmontadas[x].name,"");
        Pmontadas[x].num=0;
        strcpy(Pmontadas[x].path,"");
        x=x+1;
    }
    empezar();
    return EXIT_SUCCESS;
}
