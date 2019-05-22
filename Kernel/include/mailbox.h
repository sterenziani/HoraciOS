#include <lib.h>
#include <process.h>

#define MAILBOX_BUFFER_SIZE 4000
#define MAX_MAILBOXES 10

typedef struct mailbox{
  char name[NAME_LENGTH + 1];
  int read_index;
  int write_index;
  int empty_bytes;
  char buffer[MAILBOX_BUFFER_SIZE];
  process_id_t lockedQueue[PROCESS_QUEUE_SIZE];
  int queue_index;
}mailbox_struct_t;

typedef mailbox_struct_t* mailbox_t;

typedef struct post_office{
  int free_index;
  mailbox_t mailboxes[MAX_MAILBOXES];
}post_office_t;

void init_post_office();                                        // Crea el post office
mailbox_t create_mailbox(char* name);                           // Retorna el buzón creado o el que ya existe con ese nombre, o NULL si no se pudo crear
int write_message(mailbox_t mailbox, char* msg);                // Escribe msg en el buffer del buzón mailbox. Retorna -1 si el buzon no existe o no hay suficiente lugar para escribir
int read_message(mailbox_t mailbox, int bytes, char* dest);     // Lee n bytes del buffer del buźon mailbox. Si no hay suficiente contenido se bloquea hasta que pueda leer esa cantidad. Retorna -1 si el buzon no existe o pidio demasiados bytes.
void destroy_mailbox(mailbox_t mailbox);
int write_message_by_name(char* name, char* msg);
int read_message_by_name(char* name, int bytes, char* dest);
void destroy_mailbox_by_name(char* name);
