#ifndef DATANODE_H_
#define DATANODE_H_

/**
* @NAME: getBloque
* @DESC: Retorna el numero del bloque pedido
*/

char* getBloque (int numBLoque,char* pathFile);

/**
* @NAME: setBloque
* @DESC: escribe el numero del bloque pedido
*/

void setBloque (int numBLoque, char* bloque,char* pathFile);


#endif DATANODE_H_
