#include "medusa_pd.h"

/* -----------------------------------------------------------------------------
   MEDUSA PD CHAT RCV
------------------------------------------------------------------------------*/
void medusa_pd_chat_rcv(
      t_medusa_node * node,
      const t_medusa_message_chat * message,
      void * args
      ){
   t_medusa_pd_chat *x = (t_medusa_pd_chat *) args;
   char msg[1024];
   sprintf(msg, "%s: %s", medusa_node_get_name(node), message->data);
   while(sys_trylock());
   outlet_symbol(x->msg_symbol_outlet, gensym(msg));
   sys_unlock();
}

/* -----------------------------------------------------------------------------
   MEDUSA PD CHAT NEW
------------------------------------------------------------------------------*/
void * medusa_pd_chat_new(void) {
   t_medusa_pd_chat *x = (t_medusa_pd_chat *) pd_new(medusa_pd_chat_class);

   x->control = medusa_control_create("");
   medusa_control_set_chat_callback(x->control, medusa_pd_chat_rcv, x);
   x->msg_symbol_outlet = outlet_new(&x->x_obj, gensym("symbol"));

   return (void *) x;
}

/* -----------------------------------------------------------------------------
   MEDUSA PD CHAT MSG ANYTHING METHOD
------------------------------------------------------------------------------*/
void medusa_pd_chat_msg_anything_method(
            t_medusa_pd_chat *x,
            t_symbol *s,
            int argc,
            t_atom *argv){
   char *buf;
   int length;
   t_binbuf *b = binbuf_new();
   t_atom message;
   SETSYMBOL(&message, s);
   binbuf_add(b, 1, &message); // message name
   binbuf_add(b, argc, argv); // message parameters
   binbuf_gettext(b, &buf, &length);
   medusa_control_notify_chat(x->control, NULL, buf);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD CHAT DESTROY
------------------------------------------------------------------------------*/
void medusa_pd_chat_destroy(t_medusa_pd_chat *x){
   medusa_control_free(x->control);
}

/* -----------------------------------------------------------------------------
   MEDUSA PD CHAT SETUP
------------------------------------------------------------------------------*/
void medusa_chat_setup(void){
   medusa_pd_chat_class = class_new(gensym("medusa_chat"),
      (t_newmethod) medusa_pd_chat_new, // Constructor
      (t_method) medusa_pd_chat_destroy, // Destructor
      sizeof (t_medusa_pd_chat),
      CLASS_DEFAULT,
      0);//Must always ends with a zero

   class_addanything(medusa_pd_chat_class, medusa_pd_chat_msg_anything_method);
   class_sethelpsymbol(medusa_pd_chat_class,gensym ("medusa_chat_help"));
}

/*----------------------------------------------------------------------------*/
