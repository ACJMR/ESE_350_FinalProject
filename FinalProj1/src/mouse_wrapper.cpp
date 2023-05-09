#include <stdlib.h>
#include "mouse_wrapper.h"
#include "cppmouse.h"

struct MouseC {
    void *obj;
};

MouseC_t *mouseC_create()
{
    MouseC_t *m;
    Mouse_ * obj;

    m      = (MouseC_t*) malloc(sizeof(*m));
    obj    = new Mouse_();
    m->obj = static_cast<void *>(obj);

    return m;
}


void mouseC_move(MouseC_t *m, int x, int y)
{
    Mouse_ *obj;
    /*
    if (m == NULL)
        return;
    */
    obj = static_cast<Mouse_ *>(m->obj);
    obj->move(x, y);
}