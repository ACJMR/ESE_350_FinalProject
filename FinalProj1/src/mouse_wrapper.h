#ifndef mouse_wrapper_h
#define mouse_wrapper_h

#ifdef __cplusplus
extern "C" {
#endif

struct MouseC;
typedef struct MouseC MouseC_t;

MouseC_t *mouseC_create();
void mouseC_move(MouseC_t *m, int x, int y);

#ifdef __cplusplus
}
#endif

#endif