#include <opencv/cv.h>
#include <opencv/highgui.h>
//#include "etiquetado.h"
#define IMAGE 0
#define IR 1
#define IR_COLOR 2

/* typedef struct _KINECT_{ */
/*    XnContext *pContext; */
/*    XnNodeHandle Node_Image; */
/*    XnNodeHandle Node_Deep; */
/* }kinect; */

struct kinect;
// #define kinect void

//Inicializa todos los parámetros de la camara kinect
#ifdef __cplusplus
extern "C" 
#endif
struct kinect * knt_Create();
#ifdef __cplusplus
extern "C" 
#endif
void knt_Destroy(struct kinect *k);
#ifdef __cplusplus
extern "C" 
#endif
void knt_Init(struct kinect *k, int tipo);
//Cierra la camara kinct
#ifdef __cplusplus
extern "C" 
#endif
void knt_Stop(struct kinect *k);
#ifdef __cplusplus
extern "C" 
#endif
void knt_change_view(struct kinect *k, int tipo);

//Obtenemos la imagen de la camara, deopendiendo de el valor que ke pasemos nos devolvera IR o RGB
#ifdef __cplusplus
extern "C" 
#endif
IplImage* knt_get_image(struct kinect *k);
#ifdef __cplusplus
extern "C" 
#endif
IplImage* knt_get_image_deep(struct kinect *k);
#ifdef __cplusplus
extern "C" 
#endif
IplImage* knt_get_image_deep_color(struct kinect *k);

//XnNodeHandle crear_nodo_imagen( XnContext* pContext, XnStatus nRetVal, int tipo);

/*
XnNodeHandle crear_nodo_imagen_RGB( XnContext* pContext, XnStatus nRetVal);
IplImage* obtener_imagen_RGB(XnContext* pContext, XnStatus nRetVal, XnNodeHandle *NodoImg);
*/
//XnNodeHandle crear_nodo_profundidad( XnContext* pContext, XnStatus nRetVal);
/*
XnNodeHandle crear_nodo_IR( XnContext* pContext, XnStatus nRetVal);
IplImage* obtener_imagen_IR(XnContext* pContext, XnStatus nRetVal, XnNodeHandle *NodoImg);

*/
