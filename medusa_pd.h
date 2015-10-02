#include "m_pd.h"
#include <medusa.h>

static t_class * medusa_pd_receive_midi_class;
static t_class * medusa_pd_send_midi_class;
static t_class * medusa_pd_receive_audio_class;
static t_class * medusa_pd_send_audio_class;
static t_class * medusa_pd_chat_class;

typedef struct medusa_pd_receive_midi     t_medusa_pd_receive_midi;
typedef struct medusa_pd_send_midi        t_medusa_pd_send_midi;
typedef struct medusa_pd_receive_audio    t_medusa_pd_receive_audio;
typedef struct medusa_pd_send_audio       t_medusa_pd_send_audio;
typedef struct medusa_pd_chat             t_medusa_pd_chat;

struct medusa_pd_receive_midi{
   t_object x_obj;
   t_int number_of_channels;
   t_medusa_receiver * receiver;
   t_medusa_control * control;
   };

struct medusa_pd_send_midi{
   t_object x_obj;
   t_int number_of_channels;
   t_medusa_sender * sender;
   t_medusa_control * control;
   t_outlet * my_outlet;
   };

struct medusa_pd_receive_audio{
   t_object x_obj;
   t_int number_of_channels;
   t_sample ** outvec;
   t_medusa_receiver * receiver;
   t_medusa_control * control;
   };

struct medusa_pd_chat{
   t_object x_obj;
   t_outlet *msg_symbol_outlet;
   t_medusa_control * control;
   };

struct medusa_pd_send_audio{
   t_object x_obj;
   t_float x_f; /* inlet value when set by message */
   t_int number_of_channels;
   t_sample ** invec;
   t_medusa_sender * sender;
   t_medusa_control * control;
   };

