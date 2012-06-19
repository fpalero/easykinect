#include <XnOpenNI.h>
#include "easykinect.h"
int tipo_nodo = IMAGE;


struct kinect {
    XnContext *pContext;
    XnNodeHandle Node_Image;
    XnNodeHandle Node_Deep;
};

typedef struct kinect kinect_t;

XnNodeHandle knt_create_node_image(XnContext* pContext, int tipo);
XnNodeHandle knt_create_node_deep( XnContext* pContext);

void knt_Init(kinect_t *k, int tipo);

struct kinect * knt_Create(){
  kinect_t *k;
  k = (kinect_t *)malloc(sizeof(kinect_t));
  
  if(k == NULL){
    perror("No se puede crear la estructura kinect_t.\n");
    exit(-1);
  }
  
  k->pContext = NULL;
  
  return k;
}

void knt_Destroy(struct kinect *k){
  free(k);
}

void knt_change_view(kinect_t *k, int tipo){
     
      if((tipo_nodo == IR || tipo_nodo == IR_COLOR) && (tipo == IR || tipo == IR_COLOR)){
	tipo_nodo = tipo;
      }else{
	tipo_nodo = tipo;
	knt_Stop(k);    
	knt_Init(k, tipo_nodo);
      }
}
/********************************************/
/********************************************/
/******* Encendido/Apagado de Kinect_T ********/
XnLockHandle lock_ir;
void knt_Init(kinect_t *k, int tipo){
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = xnInit(&(k->pContext));
       
    k->Node_Deep = knt_create_node_deep( k->pContext);
    k->Node_Image = knt_create_node_image( k->pContext, tipo);
    nRetVal = xnStartGeneratingAll(k->pContext);
   
    if(nRetVal != XN_STATUS_OK){
      printf("Error ejecutando nodo: %s\n",xnGetStatusString(nRetVal));
    }
  
    
}

void knt_Stop(kinect_t *k){
     XnStatus nRetVal = XN_STATUS_OK;
     
     nRetVal = xnStopGeneratingAll(k->pContext);
     
     if(nRetVal != XN_STATUS_OK){
      printf("Error parando nodo de IR: %s\n",xnGetStatusString(nRetVal));
     }
     
      xnShutdown(k->pContext);
}

/***********************************************/
/***********************************************/
/**********   FUNCIONES NODO IMAGEN   **********/
XnNodeHandle knt_create_node_image_RGB( XnContext* pContext){
    XnStatus nRetVal = XN_STATUS_OK;
    
    XnNodeHandle NodoImg;
    XnMapOutputMode outputMode;
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error inicializando contexto: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Crear Nodo Imagen
    nRetVal = xnCreateImageGenerator(pContext, &NodoImg, NULL, NULL);

    if(nRetVal != XN_STATUS_OK){
      printf("Error creando nodo imagen: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Configuramos nodo Imagen
    outputMode.nXRes = 640;
    outputMode.nYRes = 480;
    outputMode.nFPS = 30;
    
    
    nRetVal = xnSetMapOutputMode(NodoImg, &outputMode);
    
     if(nRetVal != XN_STATUS_OK){
      printf("Error configurando nodo imagen: %s\n", xnGetStatusString(nRetVal));
    }
    
    return NodoImg;
}

IplImage* knt_get_image_RGB(kinect_t *k){
    XnStatus nRetVal = XN_STATUS_OK;
  //Creamos una imagen de profundidad de 8 bits y 3 canales
    IplImage* imgk = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    
    XnRGB24Pixel* pImghMap;
    int i = 0, j = 0;
    
    //Esperamos a que nos devuelva los datos
    nRetVal = xnWaitOneUpdateAll(k->pContext, k->Node_Image);
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error actualizando información: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Obtenemos la información
     pImghMap = xnGetRGB24ImageMap(k->Node_Image); 
      
     for( i = 0; i < 480; i++) 
       for( j = 0; j < 640; j++){
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+0] = pImghMap->nBlue;
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+1] = pImghMap->nGreen;
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+2] = pImghMap->nRed;
	 ++pImghMap;
       }
     
     return imgk;
}


/***********************************************/
/***********************************************/
/********** FUNCIONES NODO PROFUNIDAD **********/
XnNodeHandle knt_create_node_deep( XnContext* pContext){
   XnStatus nRetVal = XN_STATUS_OK;
    XnNodeHandle NodoImg;
    XnMapOutputMode outputMode;
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error inicializando contexto de profundidad: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Crear Nodo Imagen
    nRetVal = xnCreateDepthGenerator(pContext, &NodoImg, NULL, NULL);

    if(nRetVal != XN_STATUS_OK){
      printf("Error creando nodo imagen de profundidad: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Configuramos nodo Imagen
    outputMode.nXRes = 640;
    outputMode.nYRes = 480;
    outputMode.nFPS = 30;
    
    
    nRetVal = xnSetMapOutputMode(NodoImg, &outputMode);
    
     if(nRetVal != XN_STATUS_OK){
      printf("Error configurando nodo imagen de profundidad: %s\n", xnGetStatusString(nRetVal));
    }
    
    return NodoImg;
}


IplImage* knt_get_image_deep(kinect_t *k){
    XnStatus nRetVal = XN_STATUS_OK;
   //Creamos una imagen de profundidad de 8 bits y un canal
   IplImage* imgk = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    
    XnDepthPixel* pDepthMap;
    int i = 0, j = 0;
    
    //Esperamos a que nos devuelva los datos
    nRetVal = xnWaitOneUpdateAll(k->pContext, k->Node_Deep);
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error actualizando información de profundidad: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Obtenemos la información
    pDepthMap = xnGetDepthMap(k->Node_Deep); 
      
     for( i = 0; i < 480; i++) 
       for( j = 0; j < 640; j++){
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j] = *pDepthMap>>4;
	 ++pDepthMap;
       }
       
     //cvNormalize(imgk, imgk, 255, 0, CV_MINMAX);
       
     return imgk;
}

IplImage* knt_get_image_deep_color(kinect_t *k){
    XnStatus nRetVal = XN_STATUS_OK;
  //Creamos una imagen de profundidad de 8 bits y 3 canales
    IplImage* imgk = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    
    XnDepthPixel* pDepthMapColor;
    int i = 0, j = 0;
    
    //Esperamos a que nos devuelva los datos
    nRetVal = xnWaitOneUpdateAll(k->pContext, k->Node_Deep);
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error actualizando información: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Obtenemos la información
     pDepthMapColor = xnGetDepthMap(k->Node_Deep); 
      
     for( i = 0; i < 480; i++) 
       for( j = 0; j < 640; j++){
	 *pDepthMapColor=(*pDepthMapColor>>4);
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+0] = (((*pDepthMapColor>>6)&0x02)<<6)|0x2D;
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+1] = (((*pDepthMapColor>>3)&0x03)<<5)|0x1D; 
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+2] = (((*pDepthMapColor)&0x03)<<5)|0x1D;
	 ++pDepthMapColor;
       }
     
     return imgk;
}

/***********************************************/
/***********************************************/
/**********     FUNCIONES NODO IR     **********/

XnNodeHandle knt_create_node_IR( XnContext* pContext){
   XnStatus nRetVal = XN_STATUS_OK;
    XnNodeHandle NodoImg;
    XnMapOutputMode outputMode;
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error inicializando contexto de IR: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Crear Nodo Imagen
    nRetVal = xnCreateIRGenerator(pContext, &NodoImg, NULL, NULL);

    if(nRetVal != XN_STATUS_OK){
      printf("Error creando nodo imagen de IR: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Configuramos nodo Imagen
    outputMode.nXRes = 640;
    outputMode.nYRes = 480;
    outputMode.nFPS = 30;
    
    
    nRetVal = xnSetMapOutputMode(NodoImg, &outputMode);
    
     if(nRetVal != XN_STATUS_OK){
      printf("Error configurando nodo imagen de IR: %s\n", xnGetStatusString(nRetVal));
    }
     
    return NodoImg;
}

IplImage* knt_get_image_IR(kinect_t *k){
   XnStatus nRetVal = XN_STATUS_OK;
  //Creamos una imagen de profundidad de 8 bits y un canal
   IplImage* imgk = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1);
    
    XnIRPixel* pIRMap;
    int i = 0, j = 0;
    
    //Esperamos a que nos devuelva los datos
    nRetVal = xnWaitOneUpdateAll(k->pContext, k->Node_Image);
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error actualizando información de IR: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Obtenemos la información
    pIRMap = xnGetIRMap( k->Node_Image); 
      
     for( i = 0; i < 480; i++) 
       for( j = 0; j < 640; j++){
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j] = (*pIRMap)>>4;
	 ++pIRMap;
       }
       
     cvNormalize(imgk, imgk, 255, 0, CV_MINMAX);
     
     return imgk;
}

IplImage* knt_get_image_IR_color(kinect_t *k){
    XnStatus nRetVal = XN_STATUS_OK;
  //Creamos una imagen de profundidad de 8 bits y 3 canales
    IplImage* imgk = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
    
    XnIRPixel* pIRMapColor;
    int i = 0, j = 0;
    
    //Esperamos a que nos devuelva los datos
    nRetVal = xnWaitOneUpdateAll(k->pContext, k->Node_Image);
    
    if(nRetVal != XN_STATUS_OK){
      printf("Error actualizando información: %s\n", xnGetStatusString(nRetVal));
    }
    
    //Obtenemos la información
     pIRMapColor = xnGetIRMap(k->Node_Image); 
      
     for( i = 0; i < 480; i++) 
       for( j = 0; j < 640; j++){
	 *pIRMapColor=(*pIRMapColor>>4);
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+0] = (((*pIRMapColor>>6)&0x02)<<6)|0x2D;
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+1] = (((*pIRMapColor>>3)&0x03)<<5)|0x1D; 
	 ((uchar *)(imgk->imageData + i*imgk->widthStep))[j*imgk->nChannels+2] = (((*pIRMapColor)&0x03)<<5)|0x1D;
	 ++pIRMapColor;
       }
     
     return imgk;
}

/*************************************************************/
/*************************************************************/
/**********   FUNCIONES SELECCIÓN NODO IMAGEN o IR  **********/

XnNodeHandle knt_create_node_image( XnContext* pContext, int tipo){
  switch(tipo){
    case IMAGE:
      tipo_nodo=IMAGE;
      return knt_create_node_image_RGB( pContext);
      break;
    case IR:
      tipo_nodo=IR;
      return knt_create_node_IR(pContext);
      break;
    case IR_COLOR:
      tipo_nodo=IR_COLOR;
      return knt_create_node_IR(pContext);
      break;
    default:
      printf("Opción incorrecta");
      return NULL;
      break;
  }
}
IplImage* knt_get_image(kinect_t *k){
   
  switch(tipo_nodo){
    case IMAGE:
      return knt_get_image_RGB(k);
      break;
    case IR:
      return knt_get_image_IR(k);
      break;
    case IR_COLOR:
      return knt_get_image_IR_color(k);
      break;
    default:
      printf("Opción incorrecta");
      return NULL;
      break;
  }
}
